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
	void setLastModified(time_t mtime);
	void setETag(const std::string& etag);
	void setCacheControl(const std::string& cacheControl);

	// Body
	void setBody(const std::string& body);
	void appendBody(const std::string& chunk);

	// Chunked transfer encoding
	void setChunked(bool chunked);
	std::string buildChunkedResponse() const;

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
	bool _chunked;

	// Get status message for code
	std::string getStatusMessage(int code) const;
	std::string formatHttpDate(time_t time) const;
};

} // namespace HTTP
