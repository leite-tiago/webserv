/**
 * Response.hpp
 * HTTP Response class - builds HTTP responses
 */
#pragma once

#include <string>
#include <map>
#include <sstream>

namespace HTTP {

class Response {
public:
	// Constructor
	Response();
	~Response();

	// Status
	void setStatus(int code);
	void setStatus(int code, const std::string& message);

	// Headers
	void setHeader(const std::string& name, const std::string& value);
	void setContentType(const std::string& contentType);
	void setContentLength(size_t length);

	// Body
	void setBody(const std::string& body);
	void appendBody(const std::string& chunk);

	// Connection
	void setKeepAlive(bool keepAlive);

	// Build response string
	std::string build() const;

	// Getters
	int getStatusCode() const;
	const std::string& getBody() const;

	// Common responses
	static Response errorResponse(int code, const std::string& message = "");
	static Response redirect(const std::string& location, int code = 302);

	// Reset
	void clear();

private:
	int _statusCode;
	std::string _statusMessage;
	std::map<std::string, std::string> _headers;
	std::string _body;

	// Get status message for code
	std::string getStatusMessage(int code) const;
};

} // namespace HTTP
