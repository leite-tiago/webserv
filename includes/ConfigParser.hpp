/**
 * ConfigParser.hpp
 * Parser para ficheiros de configuração no estilo nginx
 * Lê e parseia server blocks, locations, e directivas
 */
#pragma once

#include "Config.hpp"
#include <string>
#include <vector>
#include <fstream>

class ConfigParser {
public:
	// Constructor
	ConfigParser();
	~ConfigParser();

	// Parse configuration file
	bool parse(const std::string& filename, Config& config);

	// Get error message
	const std::string& getError() const;

private:
	// Tokenization
	std::vector<std::string> tokenize(const std::string& content);

	// Parsing helpers
	bool parseServer(std::vector<std::string>& tokens, size_t& index, Server& server);
	bool parseLocation(std::vector<std::string>& tokens, size_t& index, Route& route);
	bool parseServerDirective(const std::string& directive, std::vector<std::string>& tokens,
	                          size_t& index, Server& server);
	bool parseLocationDirective(const std::string& directive, std::vector<std::string>& tokens,
	                           size_t& index, Route& route);

	// Utility functions
	bool expectToken(std::vector<std::string>& tokens, size_t& index, const std::string& expected);
	bool isNumber(const std::string& str);
	int toInt(const std::string& str);
	size_t toSize(const std::string& str);
	std::string readFile(const std::string& filename);
	void setError(const std::string& error);

	std::string _error;  // Error message
};
