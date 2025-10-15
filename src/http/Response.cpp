/**
 * Response.cpp
 * Implementation of HTTP Response builder
 */
#include "includes/http/Response.hpp"
#include "includes/Settings.hpp"
#include "includes/Instance.hpp"
#include <sstream>

namespace HTTP {

// Constructor
Response::Response()
	: _statusCode(200)
	, _statusMessage("OK")
	, _body("") {
}

Response::~Response() {}

// Set status
void Response::setStatus(int code) {
	_statusCode = code;
	_statusMessage = getStatusMessage(code);
}

void Response::setStatus(int code, const std::string& message) {
	_statusCode = code;
	_statusMessage = message;
}

// Set header
void Response::setHeader(const std::string& name, const std::string& value) {
	_headers[name] = value;
}

void Response::setContentType(const std::string& contentType) {
	setHeader("Content-Type", contentType);
}

void Response::setContentLength(size_t length) {
	std::ostringstream oss;
	oss << length;
	setHeader("Content-Length", oss.str());
}

// Set body
void Response::setBody(const std::string& body) {
	_body = body;
	setContentLength(_body.length());
}

void Response::appendBody(const std::string& chunk) {
	_body += chunk;
	setContentLength(_body.length());
}

// Set keep-alive
void Response::setKeepAlive(bool keepAlive) {
	if (keepAlive) {
		setHeader("Connection", "keep-alive");
	} else {
		setHeader("Connection", "close");
	}
}

// Build response string
std::string Response::build() const {
	std::ostringstream response;

	// Status line
	response << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";

	// Headers
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
	     it != _headers.end(); ++it) {
		response << it->first << ": " << it->second << "\r\n";
	}

	// Empty line separating headers from body
	response << "\r\n";

	// Body
	if (!_body.empty()) {
		response << _body;
	}

	return response.str();
}

// Getters
int Response::getStatusCode() const {
	return _statusCode;
}

const std::string& Response::getBody() const {
	return _body;
}

// Get status message for code
std::string Response::getStatusMessage(int code) const {
	Settings* settings = Instance::Get<Settings>();
	return settings->httpStatusCode(code);
}

// Common responses
Response Response::errorResponse(int code, const std::string& message) {
	Response response;
	response.setStatus(code);
	response.setContentType("text/html");

	std::ostringstream body;
	body << "<!DOCTYPE html>\n"
	     << "<html>\n"
	     << "<head><title>" << code << " " << response.getStatusMessage(code) << "</title></head>\n"
	     << "<body>\n"
	     << "<h1>" << code << " " << response.getStatusMessage(code) << "</h1>\n";

	if (!message.empty()) {
		body << "<p>" << message << "</p>\n";
	}

	body << "<hr>\n"
	     << "<p><em>webserv/1.0</em></p>\n"
	     << "</body>\n"
	     << "</html>\n";

	response.setBody(body.str());
	response.setKeepAlive(false);

	return response;
}

Response Response::redirect(const std::string& location, int code) {
	Response response;
	response.setStatus(code);
	response.setHeader("Location", location);
	response.setContentType("text/html");

	std::ostringstream body;
	body << "<!DOCTYPE html>\n"
	     << "<html>\n"
	     << "<head><title>Redirecting...</title></head>\n"
	     << "<body>\n"
	     << "<h1>Redirecting...</h1>\n"
	     << "<p>You are being redirected to <a href=\"" << location << "\">" << location << "</a></p>\n"
	     << "</body>\n"
	     << "</html>\n";

	response.setBody(body.str());

	return response;
}

// Clear response
void Response::clear() {
	_statusCode = 200;
	_statusMessage = "OK";
	_headers.clear();
	_body.clear();
}

} // namespace HTTP
