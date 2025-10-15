/**
 * ConfigParser.cpp
 * Implementação do parser de configuração
 */
#include "includes/config/ConfigParser.hpp"
#include "includes/utils/Logger.hpp"
#include "includes/utils/Utils.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>

ConfigParser::ConfigParser() : _error("") {}

ConfigParser::~ConfigParser() {}

bool ConfigParser::parse(const std::string& filename, Config& config) {
	// Ler o ficheiro
	std::string content = readFile(filename);
	if (content.empty()) {
		setError("Failed to read config file: " + filename);
		return false;
	}

	// Tokenizar
	std::vector<std::string> tokens = tokenize(content);
	if (tokens.empty()) {
		setError("Empty configuration file");
		return false;
	}

	// Parsear tokens
	size_t index = 0;
	while (index < tokens.size()) {
		const std::string& token = tokens[index];

		if (token == "server") {
			Server server;
			if (!parseServer(tokens, index, server)) {
				return false;
			}
			config.addServer(server);
		} else {
			setError("Unexpected token: " + token + " (expected 'server')");
			return false;
		}
	}

	// Validar configuração
	if (!config.isValid()) {
		setError("Invalid configuration");
		return false;
	}

	return true;
}

const std::string& ConfigParser::getError() const {
	return _error;
}

// Tokenization
std::vector<std::string> ConfigParser::tokenize(const std::string& content) {
	std::vector<std::string> tokens;
	std::string token;
	bool inComment = false;

	for (size_t i = 0; i < content.length(); ++i) {
		char c = content[i];

		// Handle comments
		if (c == '#') {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			inComment = true;
			continue;
		}

		if (c == '\n') {
			inComment = false;
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			continue;
		}

		if (inComment)
			continue;

		// Handle whitespace
		if (c == ' ' || c == '\t' || c == '\r') {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			continue;
		}

		// Handle special characters
		if (c == '{' || c == '}' || c == ';') {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			tokens.push_back(std::string(1, c));
			continue;
		}

		// Regular character
		token += c;
	}

	// Add last token
	if (!token.empty()) {
		tokens.push_back(token);
	}

	return tokens;
}

// Parse server block
bool ConfigParser::parseServer(std::vector<std::string>& tokens, size_t& index, Server& server) {
	++index; // Skip "server"

	if (!expectToken(tokens, index, "{"))
		return false;

	while (index < tokens.size() && tokens[index] != "}") {
		const std::string& directive = tokens[index];

		if (directive == "location") {
			Route route;
			if (!parseLocation(tokens, index, route)) {
				return false;
			}
			server.addRoute(route);
		} else {
			if (!parseServerDirective(directive, tokens, index, server)) {
				return false;
			}
		}
	}

	if (!expectToken(tokens, index, "}"))
		return false;

	return true;
}

// Parse location block
bool ConfigParser::parseLocation(std::vector<std::string>& tokens, size_t& index, Route& route) {
	++index; // Skip "location"

	// Get path
	if (index >= tokens.size()) {
		setError("Expected path after 'location'");
		return false;
	}
	route.setPath(tokens[index++]);

	if (!expectToken(tokens, index, "{"))
		return false;

	while (index < tokens.size() && tokens[index] != "}") {
		const std::string& directive = tokens[index];
		if (!parseLocationDirective(directive, tokens, index, route)) {
			return false;
		}
	}

	if (!expectToken(tokens, index, "}"))
		return false;

	return true;
}

// Parse server directive
bool ConfigParser::parseServerDirective(const std::string& directive,
                                       std::vector<std::string>& tokens,
                                       size_t& index,
                                       Server& server) {
	++index; // Skip directive

	if (directive == "listen") {
		if (index >= tokens.size()) {
			setError("Expected port or host:port after 'listen'");
			return false;
		}

		std::string listenValue = tokens[index++];
		size_t colonPos = listenValue.find(':');

		if (colonPos != std::string::npos) {
			// Format: host:port
			std::string host = listenValue.substr(0, colonPos);
			std::string portStr = listenValue.substr(colonPos + 1);

			server.setHost(host);

			int port = toInt(portStr);
			if (port <= 0 || port > 65535) {
				setError("Invalid port number in listen directive");
				return false;
			}
			server.addPort(port);
		} else {
			// Format: port only
			int port = toInt(listenValue);
			if (port <= 0 || port > 65535) {
				setError("Invalid port number");
				return false;
			}
			server.addPort(port);
		}

		return expectToken(tokens, index, ";");

	} else if (directive == "host") {
		if (index >= tokens.size()) {
			setError("Expected host after 'host'");
			return false;
		}
		server.setHost(tokens[index++]);
		return expectToken(tokens, index, ";");

	} else if (directive == "server_name") {
		while (index < tokens.size() && tokens[index] != ";") {
			server.addServerName(tokens[index++]);
		}
		return expectToken(tokens, index, ";");

	} else if (directive == "client_max_body_size") {
		if (index >= tokens.size()) {
			setError("Expected size after 'client_max_body_size'");
			return false;
		}
		size_t size = toSize(tokens[index++]);
		server.setMaxBodySize(size);
		return expectToken(tokens, index, ";");

	} else if (directive == "error_page") {
		if (index + 1 >= tokens.size()) {
			setError("Expected code and path after 'error_page'");
			return false;
		}
		int code = toInt(tokens[index++]);
		std::string path = tokens[index++];
		server.setErrorPage(code, path);
		return expectToken(tokens, index, ";");

	} else {
		setError("Unknown server directive: " + directive);
		return false;
	}
}

// Parse location directive
bool ConfigParser::parseLocationDirective(const std::string& directive,
                                         std::vector<std::string>& tokens,
                                         size_t& index,
                                         Route& route) {
	++index; // Skip directive

	if (directive == "allow_methods" || directive == "methods") {
		while (index < tokens.size() && tokens[index] != ";") {
			route.addAllowedMethod(tokens[index++]);
		}
		return expectToken(tokens, index, ";");

	} else if (directive == "return" || directive == "redirect") {
		if (index >= tokens.size()) {
			setError("Expected URL after '" + directive + "'");
			return false;
		}
		route.setRedirect(tokens[index++]);
		return expectToken(tokens, index, ";");

	} else if (directive == "root") {
		if (index >= tokens.size()) {
			setError("Expected path after 'root'");
			return false;
		}
		route.setRoot(tokens[index++]);
		return expectToken(tokens, index, ";");

	} else if (directive == "autoindex") {
		if (index >= tokens.size()) {
			setError("Expected on/off after 'autoindex'");
			return false;
		}
		std::string value = tokens[index++];
		route.setDirectoryListing(value == "on");
		return expectToken(tokens, index, ";");

	} else if (directive == "index") {
		while (index < tokens.size() && tokens[index] != ";") {
			route.addIndexFile(tokens[index++]);
		}
		return expectToken(tokens, index, ";");

	} else if (directive == "cgi_pass") {
		if (index >= tokens.size()) {
			setError("Expected path after 'cgi_pass'");
			return false;
		}
		route.setCgiEnabled(true);
		route.setCgiPath(tokens[index++]);
		return expectToken(tokens, index, ";");

	} else if (directive == "cgi_ext") {
		if (index >= tokens.size()) {
			setError("Expected extension after 'cgi_ext'");
			return false;
		}
		route.setCgiExtension(tokens[index++]);
		return expectToken(tokens, index, ";");

	} else if (directive == "upload_enable") {
		if (index >= tokens.size()) {
			setError("Expected on/off after 'upload_enable'");
			return false;
		}
		std::string value = tokens[index++];
		route.setUploadEnabled(value == "on");
		return expectToken(tokens, index, ";");

	} else if (directive == "upload_store" || directive == "upload_path") {
		if (index >= tokens.size()) {
			setError("Expected path after '" + directive + "'");
			return false;
		}
		route.setUploadPath(tokens[index++]);
		return expectToken(tokens, index, ";");

	} else {
		setError("Unknown location directive: " + directive);
		return false;
	}
}

// Utility functions
bool ConfigParser::expectToken(std::vector<std::string>& tokens, size_t& index, const std::string& expected) {
	if (index >= tokens.size()) {
		setError("Expected '" + expected + "' but reached end of file");
		return false;
	}
	if (tokens[index] != expected) {
		setError("Expected '" + expected + "' but got '" + tokens[index] + "'");
		return false;
	}
	++index;
	return true;
}

bool ConfigParser::isNumber(const std::string& str) {
	if (str.empty())
		return false;
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] < '0' || str[i] > '9')
			return false;
	}
	return true;
}

int ConfigParser::toInt(const std::string& str) {
	return atoi(str.c_str());
}

size_t ConfigParser::toSize(const std::string& str) {
	// Suportar sufixos: K, M, G
	std::string numStr = str;
	size_t multiplier = 1;

	if (!str.empty()) {
		char suffix = str[str.length() - 1];
		if (suffix == 'K' || suffix == 'k') {
			multiplier = 1024;
			numStr = str.substr(0, str.length() - 1);
		} else if (suffix == 'M' || suffix == 'm') {
			multiplier = 1024 * 1024;
			numStr = str.substr(0, str.length() - 1);
		} else if (suffix == 'G' || suffix == 'g') {
			multiplier = 1024 * 1024 * 1024;
			numStr = str.substr(0, str.length() - 1);
		}
	}

	return static_cast<size_t>(atoi(numStr.c_str())) * multiplier;
}

std::string ConfigParser::readFile(const std::string& filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();
}

void ConfigParser::setError(const std::string& error) {
	_error = error;
	Logger::error << error << std::endl;
}
