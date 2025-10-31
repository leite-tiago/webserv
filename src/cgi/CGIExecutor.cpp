/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecutor.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:39:52 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:39:53 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * CGIExecutor.cpp
 * Implementation of CGI Executor
 */
#include "includes/cgi/CGIExecutor.hpp"
#include "includes/http/Request.hpp"
#include "includes/http/Response.hpp"
#include "includes/config/Server.hpp"
#include "includes/config/Route.hpp"
#include "includes/utils/Logger.hpp"
#include "includes/core/Settings.hpp"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <algorithm>

namespace CGI {

// Constructor
Executor::Executor() {
}

// Destructor
Executor::~Executor() {
}

// Main execution method
HTTP::Response Executor::execute(const HTTP::Request& request,
                                 const Server* server,
                                 const Route* route,
                                 const std::string& scriptPath) {
	Logger::info << "Executing CGI script: " << scriptPath << std::endl;

	// Build environment variables
	std::map<std::string, std::string> envMap = buildEnvironment(request, server, route, scriptPath);
	char** envp = envMapToArray(envMap);

	// Create pipes for stdin/stdout
	PipeSet pipes;
	if (!createPipes(pipes)) {
		freeEnvArray(envp);
		return HTTP::Response::errorResponse(500, "Failed to create pipes for CGI");
	}

	// Fork process
	pid_t pid = fork();
	if (pid < 0) {
		// Fork failed
		Logger::error << "Failed to fork for CGI execution" << std::endl;
		closePipes(pipes);
		freeEnvArray(envp);
		return HTTP::Response::errorResponse(500, "Failed to fork CGI process");
	}

	if (pid == 0) {
		// Child process
		executeChild(route->getCgiPath(), scriptPath, pipes, envp);
		// If executeChild returns, something went wrong
		exit(1);
	}

	// Parent process
	freeEnvArray(envp);

	// Handle parent I/O with timeout (default 30 seconds)
	std::string cgiOutput = handleParent(pipes, request.getBody(), pid, 30);

	// Parse CGI output and build response
	return parseCGIOutput(cgiOutput);
}

// Build environment variables for CGI
std::map<std::string, std::string> Executor::buildEnvironment(
	const HTTP::Request& request,
	const Server* server,
	const Route* /* route */,
	const std::string& scriptPath) {

	std::map<std::string, std::string> env;

	// Required CGI variables (RFC 3875)
	env["REQUEST_METHOD"] = request.getMethod();
	env["SERVER_PROTOCOL"] = request.getVersion();

	// Server information
	const std::vector<int>& ports = server->getPorts();
	std::ostringstream portStr;
	if (!ports.empty()) {
		portStr << ports[0]; // Use first port
	} else {
		portStr << "8080"; // Default
	}
	env["SERVER_PORT"] = portStr.str();

	const std::vector<std::string>& serverNames = server->getServerNames();
	if (!serverNames.empty()) {
		env["SERVER_NAME"] = serverNames[0]; // Use first server name
	} else {
		env["SERVER_NAME"] = server->getHost();
	}

	env["SERVER_SOFTWARE"] = "webserv/1.0";

	// Gateway interface version
	env["GATEWAY_INTERFACE"] = "CGI/1.1";

	// Request URI and query string
	env["REQUEST_URI"] = request.getUri();

	if (!request.getQuery().empty()) {
		env["QUERY_STRING"] = request.getQuery();
	} else {
		env["QUERY_STRING"] = "";
	}

	// Script information (CRITICAL for CGI as per subject)
	// PATH_INFO = full path to script (not the URL path)
	env["SCRIPT_FILENAME"] = scriptPath;
	env["SCRIPT_NAME"] = getScriptName(scriptPath);
	env["PATH_INFO"] = getPathInfo(request.getPath(), scriptPath);
	env["PATH_TRANSLATED"] = scriptPath;

	// Content information
	if (request.hasHeader("Content-Type")) {
		env["CONTENT_TYPE"] = request.getContentType();
	}

	if (request.hasHeader("Content-Length")) {
		std::ostringstream lenStr;
		lenStr << request.getContentLength();
		env["CONTENT_LENGTH"] = lenStr.str();
	} else {
		env["CONTENT_LENGTH"] = "0";
	}

	// HTTP headers (convert to HTTP_* format)
	// All headers from request should be passed as HTTP_HEADER_NAME
	const std::map<std::string, std::string>& headers = request.getHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
	     it != headers.end(); ++it) {
		std::string headerName = it->first;

		// Convert to uppercase and replace - with _
		std::string envName = "HTTP_";
		for (size_t i = 0; i < headerName.length(); ++i) {
			char c = headerName[i];
			if (c == '-') {
				envName += '_';
			} else if (c >= 'a' && c <= 'z') {
				envName += (c - 32); // Convert to uppercase
			} else if (c >= 'A' && c <= 'Z') {
				envName += c;
			} else {
				envName += c;
			}
		}

		// Skip Content-Type and Content-Length (already set)
		if (envName != "HTTP_CONTENT_TYPE" && envName != "HTTP_CONTENT_LENGTH") {
			env[envName] = it->second;
		}
	}

	// Remote information (client)
	// We don't have remote addr in current implementation, use placeholder
	env["REMOTE_ADDR"] = "127.0.0.1";
	env["REMOTE_HOST"] = "localhost";

	return env;
}

// Convert environment map to array for execve
char** Executor::envMapToArray(const std::map<std::string, std::string>& envMap) {
	// Allocate array (size + 1 for NULL terminator)
	char** envp = new char*[envMap.size() + 1];
	size_t i = 0;

	for (std::map<std::string, std::string>::const_iterator it = envMap.begin();
	     it != envMap.end(); ++it, ++i) {
		std::string envStr = it->first + "=" + it->second;
		envp[i] = new char[envStr.length() + 1];
		std::strcpy(envp[i], envStr.c_str());
	}

	envp[i] = NULL; // NULL terminator
	return envp;
}

// Free environment array
void Executor::freeEnvArray(char** env) {
	if (!env) return;

	for (size_t i = 0; env[i] != NULL; ++i) {
		delete[] env[i];
	}
	delete[] env;
}

// Create pipes for CGI I/O
bool Executor::createPipes(PipeSet& pipes) {
	if (pipe(pipes.stdinPipe) < 0) {
		Logger::error << "Failed to create stdin pipe" << std::endl;
		return false;
	}

	if (pipe(pipes.stdoutPipe) < 0) {
		Logger::error << "Failed to create stdout pipe" << std::endl;
		close(pipes.stdinPipe[0]);
		close(pipes.stdinPipe[1]);
		return false;
	}

	return true;
}

// Close all pipes
void Executor::closePipes(PipeSet& pipes) {
	close(pipes.stdinPipe[0]);
	close(pipes.stdinPipe[1]);
	close(pipes.stdoutPipe[0]);
	close(pipes.stdoutPipe[1]);
}

// Execute CGI script in child process
void Executor::executeChild(const std::string& cgiPath,
                           const std::string& scriptPath,
                           const PipeSet& pipes,
                           char** envp) {
	// Redirect stdin
	dup2(pipes.stdinPipe[0], STDIN_FILENO);

	// Redirect stdout
	dup2(pipes.stdoutPipe[1], STDOUT_FILENO);

	// Close all pipe file descriptors (we have dups now)
	close(pipes.stdinPipe[0]);
	close(pipes.stdinPipe[1]);
	close(pipes.stdoutPipe[0]);
	close(pipes.stdoutPipe[1]);

	// Change to script directory (required by subject)
	std::string scriptDir;
	std::string scriptFilename;

	size_t lastSlash = scriptPath.find_last_of('/');
	if (lastSlash != std::string::npos) {
		scriptDir = scriptPath.substr(0, lastSlash);
		scriptFilename = scriptPath.substr(lastSlash + 1);

		if (chdir(scriptDir.c_str()) < 0) {
			Logger::error << "Failed to chdir to: " << scriptDir << std::endl;
			exit(1);
		}
	} else {
		scriptFilename = scriptPath;
	}

	// Prepare arguments for execve
	// argv[0] = cgi executable, argv[1] = script filename (NOT full path), argv[2] = NULL
	char* argv[3];
	argv[0] = const_cast<char*>(cgiPath.c_str());
	argv[1] = const_cast<char*>(scriptFilename.c_str()); // Just filename after chdir
	argv[2] = NULL;

	// Execute CGI
	execve(cgiPath.c_str(), argv, envp);

	// If we get here, execve failed
	Logger::error << "execve failed for CGI: " << cgiPath << std::endl;
	exit(1);
}

// Handle parent process I/O
std::string Executor::handleParent(const PipeSet& pipes,
                                  const std::string& requestBody,
                                  pid_t childPid,
                                  int timeoutSeconds) {
	// Close unused pipe ends
	close(pipes.stdinPipe[0]);  // Child reads from this
	close(pipes.stdoutPipe[1]); // Child writes to this

	// Set stdout pipe to non-blocking
	int flags = fcntl(pipes.stdoutPipe[0], F_GETFL, 0);
	fcntl(pipes.stdoutPipe[0], F_SETFL, flags | O_NONBLOCK);

	// Write request body to child stdin
	if (!requestBody.empty()) {
		size_t written = 0;
		while (written < requestBody.length()) {
			ssize_t n = write(pipes.stdinPipe[1], requestBody.c_str() + written,
			                 requestBody.length() - written);
			if (n <= 0) {
				Logger::warning << "Failed to write to CGI stdin" << std::endl;
				break;
			}
			written += n;
		}
	}

	// Close stdin pipe (signal EOF to child)
	close(pipes.stdinPipe[1]);

	// Read output from child with timeout
	std::string output;
	time_t startTime = time(NULL);

	while (true) {
		// Check timeout
		if (difftime(time(NULL), startTime) > timeoutSeconds) {
			Logger::warning << "CGI timeout - killing process" << std::endl;
			kill(childPid, SIGKILL);
			waitpid(childPid, NULL, 0);
			close(pipes.stdoutPipe[0]);
			return ""; // Timeout
		}

		// Try to read
		char buffer[4096];
		ssize_t n = read(pipes.stdoutPipe[0], buffer, sizeof(buffer));

		if (n > 0) {
			output.append(buffer, n);
		} else if (n == 0) {
			// EOF - child closed stdout
			break;
		} else {
			// EAGAIN/EWOULDBLOCK - no data available
			// Check if child is still running
			int status;
			pid_t result = waitpid(childPid, &status, WNOHANG);
			if (result > 0) {
				// Child exited
				break;
			}

			// Sleep briefly to avoid busy waiting
			usleep(10000); // 10ms
		}
	}

	// Close stdout pipe
	close(pipes.stdoutPipe[0]);

	// Wait for child to finish (should be quick now)
	int status;
	waitpid(childPid, &status, 0);

	if (WIFEXITED(status)) {
		int exitCode = WEXITSTATUS(status);
		Logger::info << "CGI exited with code: " << exitCode << std::endl;
	} else if (WIFSIGNALED(status)) {
		Logger::error << "CGI killed by signal: " << WTERMSIG(status) << std::endl;
	}

	return output;
}

// Parse CGI output into HTTP::Response
HTTP::Response Executor::parseCGIOutput(const std::string& cgiOutput) {
	if (cgiOutput.empty()) {
		return HTTP::Response::errorResponse(500, "CGI produced no output");
	}

	// CGI output format:
	// Headers\r\n\r\nBody
	// or
	// Headers\n\nBody

	size_t headerEnd = cgiOutput.find("\r\n\r\n");
	bool usesCRLF = true;

	if (headerEnd == std::string::npos) {
		headerEnd = cgiOutput.find("\n\n");
		usesCRLF = false;
		if (headerEnd == std::string::npos) {
			// No header/body separation - treat all as body
			HTTP::Response response;
			response.setStatus(200);
			response.setContentType("text/html");
			response.setBody(cgiOutput);
			return response;
		}
	}

	// Split headers and body
	std::string headersSection = cgiOutput.substr(0, headerEnd);
	std::string body = cgiOutput.substr(headerEnd + (usesCRLF ? 4 : 2));

	// Parse headers
	HTTP::Response response;
	int statusCode = 200;
	std::string contentType = "text/html";

	std::istringstream headerStream(headersSection);
	std::string line;
	std::string delimiter = usesCRLF ? "\r\n" : "\n";

	while (std::getline(headerStream, line)) {
		// Remove \r if present
		if (!line.empty() && line[line.length() - 1] == '\r') {
			line = line.substr(0, line.length() - 1);
		}

		if (line.empty()) continue;

		// Parse header: Name: Value
		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos) continue;

		std::string name = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		// Trim whitespace from value
		while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
			value = value.substr(1);
		}

		// Handle special CGI headers
		if (name == "Status") {
			// Status: 200 OK
			std::istringstream statusStream(value);
			statusStream >> statusCode;
		} else if (name == "Content-Type" || name == "Content-type") {
			contentType = value;
		} else {
			// Add other headers to response
			response.setHeader(name, value);
		}
	}

	// Build response
	response.setStatus(statusCode);
	response.setContentType(contentType);
	response.setBody(body);

	return response;
}

// Helper: Get PATH_INFO from request path and script path
std::string Executor::getPathInfo(const std::string& requestPath, const std::string& /* scriptPath */) {
	// PATH_INFO is the part of URL after the script name
	// For now, return the request path
	// This might need adjustment based on actual routing
	return requestPath;
}

// Helper: Get script name from full path
std::string Executor::getScriptName(const std::string& scriptPath) {
	size_t lastSlash = scriptPath.find_last_of('/');
	if (lastSlash != std::string::npos) {
		return scriptPath.substr(lastSlash + 1);
	}
	return scriptPath;
}

} // namespace CGI
