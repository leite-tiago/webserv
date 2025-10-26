/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIExecutor.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:40:46 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:40:47 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * CGIExecutor.hpp
 * CGI (Common Gateway Interface) Executor
 * Handles execution of CGI scripts (PHP, Python, etc.)
 */
#pragma once

#include <string>
#include <map>
#include <vector>

// Forward declarations
namespace HTTP {
	class Request;
	class Response;
}

class Server;
class Route;

namespace CGI {

class Executor {
public:
	// Constructor & Destructor
	Executor();
	~Executor();

	// Main execution method
	// Returns HTTP::Response with CGI output
	HTTP::Response execute(const HTTP::Request& request,
	                       const Server* server,
	                       const Route* route,
	                       const std::string& scriptPath);

private:
	// Environment setup
	std::map<std::string, std::string> buildEnvironment(
		const HTTP::Request& request,
		const Server* server,
		const Route* route,
		const std::string& scriptPath
	);

	// Convert map to char** for execve
	char** envMapToArray(const std::map<std::string, std::string>& envMap);
	void freeEnvArray(char** env);

	// Process management
	struct PipeSet {
		int stdinPipe[2];   // Parent writes to child stdin
		int stdoutPipe[2];  // Parent reads from child stdout
	};

	bool createPipes(PipeSet& pipes);
	void closePipes(PipeSet& pipes);

	// Execute CGI script in child process
	void executeChild(const std::string& cgiPath,
	                 const std::string& scriptPath,
	                 const PipeSet& pipes,
	                 char** envp);

	// Handle parent process I/O
	std::string handleParent(const PipeSet& pipes,
	                        const std::string& requestBody,
	                        pid_t childPid,
	                        int timeoutSeconds);

	// Parse CGI output
	HTTP::Response parseCGIOutput(const std::string& cgiOutput);

	// Timeout handling
	bool waitWithTimeout(pid_t pid, int timeoutSeconds, int& status);

	// Helper methods
	std::string getPathInfo(const std::string& requestPath, const std::string& scriptPath);
	std::string getScriptName(const std::string& scriptPath);
	std::string resolveScriptPath(const std::string& requestPath,
	                              const Route* route);
};

} // namespace CGI
