/**
 * Request.hpp
 * HTTP Request class - parses and stores HTTP request data
 */
#pragma once

#include <string>
#include <map>
#include <vector>

namespace HTTP {

class Request {
public:
	// Constructor
	Request();
	~Request();

	// Parsing
	bool parse(const std::string& rawRequest);
	bool isComplete() const;

	// Getters
	const std::string& getMethod() const;
	const std::string& getUri() const;
	const std::string& getPath() const;
	const std::string& getQuery() const;
	const std::string& getVersion() const;
	const std::map<std::string, std::string>& getHeaders() const;
	std::string getHeader(const std::string& name) const;
	const std::string& getBody() const;

	// Content properties
	size_t getContentLength() const;
	std::string getContentType() const;
	bool hasHeader(const std::string& name) const;

	// Connection properties
	bool keepAlive() const;

	// Debug
	void print() const;

	// Reset
	void clear();

private:
	// Request line
	std::string _method;     // GET, POST, DELETE, etc.
	std::string _uri;        // Full URI (/path?query)
	std::string _path;       // Path part (/path)
	std::string _query;      // Query string (query)
	std::string _version;    // HTTP/1.1

	// Headers
	std::map<std::string, std::string> _headers;

	// Body
	std::string _body;

	// Parsing state
	bool _complete;
	size_t _contentLength;
	bool _hasContentLength;

	// Parsing helpers
	bool parseRequestLine(const std::string& line);
	bool parseHeader(const std::string& line);
	void parseUri(const std::string& uri);
	std::string toLowerCase(const std::string& str) const;
	std::string trim(const std::string& str) const;
};

} // namespace HTTP
