/**
 * Config.cpp
 * Implementação da classe Config
 */
#include "includes/config/Config.hpp"
#include "includes/utils/Logger.hpp"
#include <iostream>

// Constructors
Config::Config() {}

Config::~Config() {}

Config::Config(const Config& other) {
	*this = other;
}

Config& Config::operator=(const Config& other) {
	if (this != &other) {
		_servers = other._servers;
	}
	return *this;
}

// Server management
void Config::addServer(const Server& server) {
	_servers.push_back(server);
}

const std::vector<Server>& Config::getServers() const {
	return _servers;
}

// Server lookup
const Server* Config::getServer(const std::string& host, int port, const std::string& serverName) const {
	// 1. Procurar server com host:port e serverName matching
	if (!serverName.empty()) {
		for (size_t i = 0; i < _servers.size(); ++i) {
			const Server& server = _servers[i];

			// Verificar se o host e porta coincidem
			if (server.getHost() != host)
				continue;

			const std::vector<int>& ports = server.getPorts();
			bool portMatch = false;
			for (size_t j = 0; j < ports.size(); ++j) {
				if (ports[j] == port) {
					portMatch = true;
					break;
				}
			}
			if (!portMatch)
				continue;

			// Verificar se o serverName coincide
			const std::vector<std::string>& serverNames = server.getServerNames();
			for (size_t j = 0; j < serverNames.size(); ++j) {
				if (serverNames[j] == serverName) {
					return &server;
				}
			}
		}
	}

	// 2. Se não encontrou com serverName, retornar o default server
	return getDefaultServer(host, port);
}

const Server* Config::getDefaultServer(const std::string& host, int port) const {
	// Procurar o primeiro server (default) para este host:port
	for (size_t i = 0; i < _servers.size(); ++i) {
		const Server& server = _servers[i];

		// Verificar se o host coincide
		if (server.getHost() != host)
			continue;

		// Verificar se a porta coincide
		const std::vector<int>& ports = server.getPorts();
		for (size_t j = 0; j < ports.size(); ++j) {
			if (ports[j] == port) {
				return &server;
			}
		}
	}

	return NULL;
}

// Validation
bool Config::isValid() const {
	// Precisa de pelo menos um server
	if (_servers.empty())
		return false;

	// Verificar se todos os servers são válidos
	for (size_t i = 0; i < _servers.size(); ++i) {
		if (!_servers[i].isValid())
			return false;
	}

	return true;
}

// Debug
void Config::print() const {
	std::cout << "=== Configuration ===" << std::endl;
	std::cout << "Total servers: " << _servers.size() << std::endl;
	std::cout << std::endl;

	for (size_t i = 0; i < _servers.size(); ++i) {
		std::cout << "--- Server " << (i + 1) << " ---" << std::endl;
		_servers[i].print();
		std::cout << std::endl;
	}
}
