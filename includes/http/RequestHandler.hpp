/**
 * RequestHandler.hpp
 * Handles HTTP requests and generates responses
 */
#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "../Server.hpp"
#include "../Route.hpp"

namespace HTTP {

class RequestHandler {
public:
	// Constructor
	RequestHandler(const Server* server);
	~RequestHandler();

	// Handle request
	Response handle(const Request& request);

private:
	const Server* _server;

	// Method handlers
	Response handleGet(const Request& request, const Route* route);
	Response handlePost(const Request& request, const Route* route);
	Response handleDelete(const Request& request, const Route* route);

	// Helper methods
	std::string resolveFilePath(const std::string& path, const Route* route);
	std::string getFileExtension(const std::string& path);
	bool fileExists(const std::string& path);
	bool isDirectory(const std::string& path);
	std::string readFile(const std::string& path);
	std::string generateDirectoryListing(const std::string& path, const std::string& requestPath);

	// Error responses
	Response notFound(const std::string& path);
	Response forbidden(const std::string& path);
	Response methodNotAllowed(const std::string& method);
	Response internalServerError(const std::string& message);
};

} // namespace HTTP
