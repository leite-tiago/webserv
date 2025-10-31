/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:38:36 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:37 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	, _hasContentLength(false)
	, _isChunked(false) {
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

	// Check URI length limit
	if (_uri.length() > MAX_URI_LENGTH) {
		Logger::error << "URI too long: " << _uri.length() << " bytes (max: " << MAX_URI_LENGTH << ")" << std::endl;
		return false;
	}

	// Parse headers
	size_t i = 1;
	size_t headerCount = 0;
	for (; i < lines.size(); ++i) {
		if (lines[i].empty()) {
			// Empty line marks end of headers
			++i;
			break;
		}

		// Check header count limit
		if (headerCount >= MAX_HEADERS_COUNT) {
			Logger::error << "Too many headers: " << headerCount << " (max: " << MAX_HEADERS_COUNT << ")" << std::endl;
			return false;
		}

		// Check header size limit
		if (lines[i].length() > MAX_HEADER_SIZE) {
			Logger::error << "Header too long: " << lines[i].length() << " bytes (max: " << MAX_HEADER_SIZE << ")" << std::endl;
			return false;
		}

		if (!parseHeader(lines[i])) {
			Logger::warning << "Failed to parse header: " << lines[i] << std::endl;
		}
		++headerCount;
	}

	// Check for Transfer-Encoding: chunked
	if (hasHeader("transfer-encoding")) {
		std::string te = getHeader("transfer-encoding");
		if (toLowerCase(te).find("chunked") != std::string::npos) {
			_isChunked = true;
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

	// If chunked, decode the body
	if (_isChunked) {
		_body = decodeChunkedBody(_body);
		_complete = true; // Chunked body is complete when we have all chunks
	} else if (_hasContentLength) {
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

	// URL decode the path
	_path = urlDecode(_path);
}

// Decode chunked transfer encoding
std::string Request::decodeChunkedBody(const std::string& chunkedData) {
	std::string result;
	std::istringstream stream(chunkedData);
	std::string line;

	while (std::getline(stream, line)) {
		// Remove \r if present
		if (!line.empty() && line[line.length() - 1] == '\r') {
			line = line.substr(0, line.length() - 1);
		}

		// Parse chunk size (hex number)
		if (line.empty()) {
			continue;
		}

		// Convert hex string to size
		size_t chunkSize = 0;
		std::istringstream hexStream(line);
		hexStream >> std::hex >> chunkSize;

		if (chunkSize == 0) {
			// Last chunk, we're done
			break;
		}

		// Read chunk data
		std::vector<char> buffer(chunkSize);
		stream.read(&buffer[0], chunkSize);
		result.append(&buffer[0], stream.gcount());

		// Skip trailing \r\n after chunk data
		std::getline(stream, line);
	}

	return result;
}

// URL decode (convert %XX to characters)
std::string Request::urlDecode(const std::string& str) const {
	std::string result;
	result.reserve(str.length());

	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '%' && i + 2 < str.length()) {
			// Convert %XX to character
			std::string hex = str.substr(i + 1, 2);
			char ch = static_cast<char>(strtol(hex.c_str(), NULL, 16));
			result += ch;
			i += 2;
		} else if (str[i] == '+') {
			// Convert + to space (for form data)
			result += ' ';
		} else {
			result += str[i];
		}
	}

	return result;
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

bool Request::isChunked() const {
	return _isChunked;
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

// Parse query string into key-value pairs
std::map<std::string, std::string> Request::getQueryParams() const {
	std::map<std::string, std::string> params;

	if (_query.empty()) {
		return params;
	}

	// Split by &
	std::istringstream stream(_query);
	std::string pair;

	while (std::getline(stream, pair, '&')) {
		size_t eqPos = pair.find('=');
		if (eqPos != std::string::npos) {
			std::string key = urlDecode(pair.substr(0, eqPos));
			std::string value = urlDecode(pair.substr(eqPos + 1));
			params[key] = value;
		} else {
			// Parameter without value
			params[urlDecode(pair)] = "";
		}
	}

	return params;
}

std::string Request::getQueryParam(const std::string& name) const {
	std::map<std::string, std::string> params = getQueryParams();
	std::map<std::string, std::string>::const_iterator it = params.find(name);
	if (it != params.end()) {
		return it->second;
	}
	return "";
}

// Parse form data (application/x-www-form-urlencoded)
std::map<std::string, std::string> Request::getFormData() const {
	std::map<std::string, std::string> formData;

	std::string contentType = getContentType();
	if (contentType.find("application/x-www-form-urlencoded") == std::string::npos) {
		return formData;
	}

	// Parse body as query string
	std::istringstream stream(_body);
	std::string pair;

	while (std::getline(stream, pair, '&')) {
		size_t eqPos = pair.find('=');
		if (eqPos != std::string::npos) {
			std::string key = urlDecode(pair.substr(0, eqPos));
			std::string value = urlDecode(pair.substr(eqPos + 1));
			formData[key] = value;
		} else {
			formData[urlDecode(pair)] = "";
		}
	}

	return formData;
}

std::string Request::getFormField(const std::string& name) const {
	std::map<std::string, std::string> formData = getFormData();
	std::map<std::string, std::string>::const_iterator it = formData.find(name);
	if (it != formData.end()) {
		return it->second;
	}
	return "";
}

// Check if request is multipart
bool Request::isMultipart() const {
	std::string contentType = getContentType();
	return contentType.find("multipart/form-data") != std::string::npos;
}

// Get multipart boundary
std::string Request::getMultipartBoundary() const {
	if (!isMultipart()) {
		return "";
	}

	std::string contentType = getContentType();
	size_t boundaryPos = contentType.find("boundary=");
	if (boundaryPos == std::string::npos) {
		return "";
	}

	std::string boundary = contentType.substr(boundaryPos + 9); // 9 = length of "boundary="

	// Remove quotes if present
	if (!boundary.empty() && boundary[0] == '"') {
		boundary = boundary.substr(1);
	}
	if (!boundary.empty() && boundary[boundary.length() - 1] == '"') {
		boundary = boundary.substr(0, boundary.length() - 1);
	}

	return boundary;
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
	_isChunked = false;
}

} // namespace HTTP
