/**
 * RequestHandler.cpp
 * Implementation of HTTP Request Handler
 */
#include "includes/http/RequestHandler.hpp"
#include "includes/Settings.hpp"
#include "includes/Instance.hpp"
#include "includes/utils/Logger.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <sstream>

namespace HTTP {

// Constructor
RequestHandler::RequestHandler(const Server* server)
	: _server(server) {
}

RequestHandler::~RequestHandler() {}

// Handle request
Response RequestHandler::handle(const Request& request) {
	Logger::info << "Handling " << request.getMethod() << " " << request.getPath() << std::endl;

	// Find matching route
	const Route* route = _server->matchRoute(request.getPath());
	if (!route) {
		Logger::warning << "No route found for path: " << request.getPath() << std::endl;
		return notFound(request.getPath());
	}

	// Check if method is allowed
	if (!route->isMethodAllowed(request.getMethod())) {
		Logger::warning << "Method " << request.getMethod() << " not allowed for path: "
		                << request.getPath() << std::endl;
		return methodNotAllowed(request.getMethod());
	}

	// Check for redirect
	if (!route->getRedirect().empty()) {
		Logger::info << "Redirecting to: " << route->getRedirect() << std::endl;
		return Response::redirect(route->getRedirect(), 301);
	}

	// Handle based on method
	if (request.getMethod() == "GET") {
		return handleGet(request, route);
	} else if (request.getMethod() == "POST") {
		return handlePost(request, route);
	} else if (request.getMethod() == "DELETE") {
		return handleDelete(request, route);
	} else {
		return methodNotAllowed(request.getMethod());
	}
}

// Handle GET request
Response RequestHandler::handleGet(const Request& request, const Route* route) {
	std::string filePath = resolveFilePath(request.getPath(), route);

	Logger::debug << "Resolved file path: " << filePath << std::endl;

	// Check if file exists
	if (!fileExists(filePath)) {
		return notFound(request.getPath());
	}

	// Check if it's a directory
	if (isDirectory(filePath)) {
		// Try index files first
		const std::vector<std::string>& indexFiles = route->getIndexFiles();
		for (size_t i = 0; i < indexFiles.size(); ++i) {
			std::string indexPath = filePath;
			if (indexPath[indexPath.length() - 1] != '/') {
				indexPath += "/";
			}
			indexPath += indexFiles[i];

			if (fileExists(indexPath) && !isDirectory(indexPath)) {
				filePath = indexPath;
				break;
			}
		}

		// If still a directory, check if autoindex is enabled
		if (isDirectory(filePath)) {
			if (route->isDirectoryListingEnabled()) {
				return Response::errorResponse(200, generateDirectoryListing(filePath, request.getPath()));
			} else {
				return forbidden("Directory listing is disabled");
			}
		}
	}

	// Read file
	std::string content = readFile(filePath);
	if (content.empty() && !fileExists(filePath)) {
		return internalServerError("Failed to read file");
	}

	// Build response
	Response response;
	response.setStatus(200);

	// Set content type based on file extension
	std::string extension = getFileExtension(filePath);
	Settings* settings = Instance::Get<Settings>();
	response.setContentType(settings->httpMimeType(extension));

	response.setBody(content);
	response.setKeepAlive(false); // For now, always close connection

	Logger::success << "Served file: " << filePath << " (" << content.length() << " bytes)" << std::endl;

	return response;
}

// Handle POST request
Response RequestHandler::handlePost(const Request& request, const Route* route) {
	// Check if upload is enabled
	if (route->isUploadEnabled()) {
		// TODO: Implement file upload
		Logger::info << "File upload requested (not yet implemented)" << std::endl;
		return Response::errorResponse(501, "File upload not yet implemented");
	}

	// Check if CGI is enabled
	if (route->isCgiEnabled()) {
		// TODO: Implement CGI
		Logger::info << "CGI execution requested (not yet implemented)" << std::endl;
		return Response::errorResponse(501, "CGI not yet implemented");
	}

	// Simple POST response
	Response response;
	response.setStatus(200);
	response.setContentType("text/html");

	std::ostringstream body;
	body << "<!DOCTYPE html>\n"
	     << "<html>\n"
	     << "<head><title>POST Received</title></head>\n"
	     << "<body>\n"
	     << "<h1>POST Request Received</h1>\n"
	     << "<p>Body size: " << request.getBody().length() << " bytes</p>\n"
	     << "</body>\n"
	     << "</html>\n";

	response.setBody(body.str());
	response.setKeepAlive(false);

	return response;
}

// Handle DELETE request
Response RequestHandler::handleDelete(const Request& request, const Route* route) {
	std::string filePath = resolveFilePath(request.getPath(), route);

	Logger::debug << "Attempting to delete: " << filePath << std::endl;

	// Check if file exists
	if (!fileExists(filePath)) {
		return notFound(request.getPath());
	}

	// Check if it's a directory
	if (isDirectory(filePath)) {
		return forbidden("Cannot delete directories");
	}

	// Try to delete file
	if (unlink(filePath.c_str()) == 0) {
		Logger::success << "Deleted file: " << filePath << std::endl;

		Response response;
		response.setStatus(204); // No Content
		response.setKeepAlive(false);
		return response;
	} else {
		Logger::error << "Failed to delete file: " << filePath << std::endl;
		return internalServerError("Failed to delete file");
	}
}

// Resolve file path
std::string RequestHandler::resolveFilePath(const std::string& requestPath, const Route* route) {
	std::string root = route->getRoot();
	std::string routePath = route->getPath();

	// Remove route prefix from request path
	std::string relativePath = requestPath;
	if (relativePath.compare(0, routePath.length(), routePath) == 0) {
		relativePath = relativePath.substr(routePath.length());
	}

	// Build full path
	std::string fullPath = root;
	if (!fullPath.empty() && fullPath[fullPath.length() - 1] != '/' &&
	    (relativePath.empty() || relativePath[0] != '/')) {
		fullPath += "/";
	}
	fullPath += relativePath;

	return fullPath;
}

// Get file extension
std::string RequestHandler::getFileExtension(const std::string& path) {
	size_t dotPos = path.find_last_of('.');
	if (dotPos != std::string::npos && dotPos < path.length() - 1) {
		return path.substr(dotPos + 1);
	}
	return "";
}

// Check if file exists
bool RequestHandler::fileExists(const std::string& path) {
	return access(path.c_str(), F_OK) == 0;
}

// Check if path is a directory
bool RequestHandler::isDirectory(const std::string& path) {
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf) != 0) {
		return false;
	}
	return S_ISDIR(statbuf.st_mode);
}

// Read file content
std::string RequestHandler::readFile(const std::string& path) {
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open()) {
		return "";
	}

	std::ostringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}

// Generate directory listing HTML
std::string RequestHandler::generateDirectoryListing(const std::string& dirPath, const std::string& requestPath) {
	std::ostringstream html;

	html << "<!DOCTYPE html>\n"
	     << "<html>\n"
	     << "<head><title>Index of " << requestPath << "</title></head>\n"
	     << "<body>\n"
	     << "<h1>Index of " << requestPath << "</h1>\n"
	     << "<hr>\n"
	     << "<ul>\n";

	// Add parent directory link if not root
	if (requestPath != "/") {
		html << "<li><a href=\"../\">../</a></li>\n";
	}

	// Open directory
	DIR* dir = opendir(dirPath.c_str());
	if (dir) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			std::string name = entry->d_name;

			// Skip . and ..
			if (name == "." || name == "..") {
				continue;
			}

			// Check if it's a directory
			std::string fullPath = dirPath;
			if (fullPath[fullPath.length() - 1] != '/') {
				fullPath += "/";
			}
			fullPath += name;

			if (isDirectory(fullPath)) {
				html << "<li><a href=\"" << name << "/\">" << name << "/</a></li>\n";
			} else {
				html << "<li><a href=\"" << name << "\">" << name << "</a></li>\n";
			}
		}
		closedir(dir);
	}

	html << "</ul>\n"
	     << "<hr>\n"
	     << "<p><em>webserv/1.0</em></p>\n"
	     << "</body>\n"
	     << "</html>\n";

	return html.str();
}

// Error responses
Response RequestHandler::notFound(const std::string& path) {
	return Response::errorResponse(404, "The requested URL " + path + " was not found on this server.");
}

Response RequestHandler::forbidden(const std::string& message) {
	return Response::errorResponse(403, message);
}

Response RequestHandler::methodNotAllowed(const std::string& method) {
	return Response::errorResponse(405, "Method " + method + " is not allowed for this resource.");
}

Response RequestHandler::internalServerError(const std::string& message) {
	return Response::errorResponse(500, message);
}

} // namespace HTTP
