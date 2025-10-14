/**
 * Request.cpp
 * Implementation of HTTP Request parser
 */
#include "includes/http/Request.hpp"
#include "includes/utils/Logger.hpp"
#include <sstream>
#include <algorithm>

namespace HTTP {

// Constructor
Request::Request()
	: _method("")
	, _uri("")
	, _path("")
	, _query("")
	, _version("")
	, _body("")
	, _complete(false)
	, _contentLength(0)
	, _hasContentLength(false) {
}

Request::~Request() {}

// Parse HTTP request
bool Request::parse(const std::string& rawRequest) {
	if (rawRequest.empty()) {
		return false;
	}

	// Split into lines
	std::vector<std::string> lines;
	std::istringstream stream(rawRequest);
	std::string line;

	while (std::getline(stream, line)) {
		// Remove \r if present
		if (!line.empty() && line[line.length() - 1] == '\r') {
			line = line.substr(0, line.length() - 1);
		}
		lines.push_back(line);
	}

	if (lines.empty()) {
		return false;
	}

	// Parse request line (first line)
	if (!parseRequestLine(lines[0])) {
		Logger::error << "Failed to parse request line: " << lines[0] << std::endl;
		return false;
	}

	// Parse headers
	size_t i = 1;
	for (; i < lines.size(); ++i) {
		if (lines[i].empty()) {
			// Empty line marks end of headers
			++i;
			break;
		}
		if (!parseHeader(lines[i])) {
			Logger::warning << "Failed to parse header: " << lines[i] << std::endl;
		}
	}

	// Check for Content-Length header
	if (hasHeader("content-length")) {
		std::string clStr = getHeader("content-length");
		_contentLength = static_cast<size_t>(atoi(clStr.c_str()));
		_hasContentLength = true;
	}

	// Rest is body
	for (; i < lines.size(); ++i) {
		_body += lines[i];
		if (i < lines.size() - 1) {
			_body += "\n";
		}
	}

	// Check if request is complete
	if (_hasContentLength) {
		_complete = (_body.length() >= _contentLength);
	} else {
		_complete = true; // No body expected
	}

	return true;
}

// Parse request line (e.g., "GET /path HTTP/1.1")
bool Request::parseRequestLine(const std::string& line) {
	std::istringstream iss(line);

	if (!(iss >> _method >> _uri >> _version)) {
		return false;
	}

	// Parse URI into path and query
	parseUri(_uri);

	return true;
}

// Parse header line (e.g., "Host: localhost")
bool Request::parseHeader(const std::string& line) {
	size_t colonPos = line.find(':');
	if (colonPos == std::string::npos) {
		return false;
	}

	std::string name = trim(line.substr(0, colonPos));
	std::string value = trim(line.substr(colonPos + 1));

	// Store header with lowercase name for case-insensitive lookup
	_headers[toLowerCase(name)] = value;

	return true;
}

// Parse URI into path and query
void Request::parseUri(const std::string& uri) {
	size_t queryPos = uri.find('?');

	if (queryPos != std::string::npos) {
		_path = uri.substr(0, queryPos);
		_query = uri.substr(queryPos + 1);
	} else {
		_path = uri;
		_query = "";
	}
}

// Convert string to lowercase
std::string Request::toLowerCase(const std::string& str) const {
	std::string result = str;
	for (size_t i = 0; i < result.length(); ++i) {
		result[i] = std::tolower(result[i]);
	}
	return result;
}

// Trim whitespace from string
std::string Request::trim(const std::string& str) const {
	size_t start = 0;
	size_t end = str.length();

	// Trim from start
	while (start < end && std::isspace(str[start])) {
		++start;
	}

	// Trim from end
	while (end > start && std::isspace(str[end - 1])) {
		--end;
	}

	return str.substr(start, end - start);
}

// Check if request is complete
bool Request::isComplete() const {
	return _complete;
}

// Getters
const std::string& Request::getMethod() const { return _method; }
const std::string& Request::getUri() const { return _uri; }
const std::string& Request::getPath() const { return _path; }
const std::string& Request::getQuery() const { return _query; }
const std::string& Request::getVersion() const { return _version; }
const std::map<std::string, std::string>& Request::getHeaders() const { return _headers; }
const std::string& Request::getBody() const { return _body; }

std::string Request::getHeader(const std::string& name) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(toLowerCase(name));
	if (it != _headers.end()) {
		return it->second;
	}
	return "";
}

size_t Request::getContentLength() const {
	return _contentLength;
}

std::string Request::getContentType() const {
	return getHeader("content-type");
}

bool Request::hasHeader(const std::string& name) const {
	return _headers.find(toLowerCase(name)) != _headers.end();
}

bool Request::keepAlive() const {
	std::string connection = getHeader("connection");

	// HTTP/1.1 defaults to keep-alive
	if (_version == "HTTP/1.1") {
		return (toLowerCase(connection) != "close");
	}

	// HTTP/1.0 defaults to close
	return (toLowerCase(connection) == "keep-alive");
}

// Debug print
void Request::print() const {
	std::cout << "=== HTTP Request ===" << std::endl;
	std::cout << "Method: " << _method << std::endl;
	std::cout << "URI: " << _uri << std::endl;
	std::cout << "Path: " << _path << std::endl;
	std::cout << "Query: " << _query << std::endl;
	std::cout << "Version: " << _version << std::endl;
	std::cout << "Headers:" << std::endl;

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
	     it != _headers.end(); ++it) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}

	if (!_body.empty()) {
		std::cout << "Body (" << _body.length() << " bytes):" << std::endl;
		std::cout << _body << std::endl;
	}
	std::cout << "===================" << std::endl;
}

// Clear request
void Request::clear() {
	_method.clear();
	_uri.clear();
	_path.clear();
	_query.clear();
	_version.clear();
	_headers.clear();
	_body.clear();
	_complete = false;
	_contentLength = 0;
	_hasContentLength = false;
}

} // namespace HTTP
