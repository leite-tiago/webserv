/**
 * Server.cpp
 * Implementação da classe Server
 */
#include "includes/config/Server.hpp"
#include "includes/utils/Logger.hpp"
#include <iostream>
#include <algorithm>

// Constructors
Server::Server()
	: _host("0.0.0.0")
	, _maxBodySize(1048576) // 1MB default
	, _isDefaultServer(false) {
}

Server::~Server() {}

Server::Server(const Server& other) {
	*this = other;
}

Server& Server::operator=(const Server& other) {
	if (this != &other) {
		_ports = other._ports;
		_host = other._host;
		_serverNames = other._serverNames;
		_maxBodySize = other._maxBodySize;
		_errorPages = other._errorPages;
		_routes = other._routes;
		_isDefaultServer = other._isDefaultServer;
	}
	return *this;
}

// Getters
const std::vector<int>& Server::getPorts() const { return _ports; }
const std::string& Server::getHost() const { return _host; }
const std::vector<std::string>& Server::getServerNames() const { return _serverNames; }
size_t Server::getMaxBodySize() const { return _maxBodySize; }
const std::map<int, std::string>& Server::getErrorPages() const { return _errorPages; }
const std::vector<Route>& Server::getRoutes() const { return _routes; }
bool Server::isDefaultServer() const { return _isDefaultServer; }

// Setters
void Server::addPort(int port) {
	// Verificar se já existe
	for (size_t i = 0; i < _ports.size(); ++i) {
		if (_ports[i] == port)
			return;
	}
	_ports.push_back(port);
}

void Server::setHost(const std::string& host) {
	_host = host;
}

void Server::addServerName(const std::string& serverName) {
	_serverNames.push_back(serverName);
}

void Server::setMaxBodySize(size_t size) {
	_maxBodySize = size;
}

void Server::setErrorPage(int code, const std::string& path) {
	_errorPages[code] = path;
}

void Server::addRoute(const Route& route) {
	_routes.push_back(route);
}

void Server::setDefaultServer(bool isDefault) {
	_isDefaultServer = isDefault;
}

// Route matching
const Route* Server::matchRoute(const std::string& path) const {
	// Procurar a route que melhor corresponde ao path
	// Algoritmo: procurar o longest prefix match
	const Route* bestMatch = NULL;
	size_t bestMatchLen = 0;

	for (size_t i = 0; i < _routes.size(); ++i) {
		const std::string& routePath = _routes[i].getPath();

		// Verificar se o path começa com routePath
		if (path.compare(0, routePath.length(), routePath) == 0) {
			// É um match! Verificar se é melhor que o anterior
			if (routePath.length() > bestMatchLen) {
				bestMatch = &_routes[i];
				bestMatchLen = routePath.length();
			}
		}
	}

	// Se não encontrou nenhum match, retornar a route "/"  se existir
	if (bestMatch == NULL) {
		for (size_t i = 0; i < _routes.size(); ++i) {
			if (_routes[i].getPath() == "/") {
				bestMatch = &_routes[i];
				break;
			}
		}
	}

	return bestMatch;
}

// Error page retrieval
std::string Server::getErrorPage(int code) const {
	std::map<int, std::string>::const_iterator it = _errorPages.find(code);
	if (it != _errorPages.end()) {
		return it->second;
	}
	return ""; // Empty string significa usar default error page
}

// Validation
bool Server::isValid() const {
	// Um server precisa de pelo menos uma porta
	if (_ports.empty())
		return false;

	// Verificar se todas as routes são válidas
	for (size_t i = 0; i < _routes.size(); ++i) {
		if (!_routes[i].isValid())
			return false;
	}

	return true;
}

// Debug
void Server::print() const {
	std::cout << "Server:" << std::endl;
	std::cout << "  Host: " << _host << std::endl;

	std::cout << "  Ports: ";
	for (size_t i = 0; i < _ports.size(); ++i) {
		std::cout << _ports[i];
		if (i < _ports.size() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;

	if (!_serverNames.empty()) {
		std::cout << "  Server names: ";
		for (size_t i = 0; i < _serverNames.size(); ++i) {
			std::cout << _serverNames[i];
			if (i < _serverNames.size() - 1)
				std::cout << ", ";
		}
		std::cout << std::endl;
	}

	std::cout << "  Max body size: " << _maxBodySize << " bytes" << std::endl;
	std::cout << "  Default server: " << (_isDefaultServer ? "yes" : "no") << std::endl;

	if (!_errorPages.empty()) {
		std::cout << "  Error pages:" << std::endl;
		for (std::map<int, std::string>::const_iterator it = _errorPages.begin();
		     it != _errorPages.end(); ++it) {
			std::cout << "    " << it->first << ": " << it->second << std::endl;
		}
	}

	if (!_routes.empty()) {
		std::cout << "  Routes:" << std::endl;
		for (size_t i = 0; i < _routes.size(); ++i) {
			_routes[i].print();
		}
	}
}
