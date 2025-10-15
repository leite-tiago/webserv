/**
 * Server.hpp
 * Representa um server block de configuração
 * Cada server pode ter múltiplas portas, hosts, e routes
 */
#pragma once

#include "Route.hpp"
#include <string>
#include <vector>
#include <map>

class Server {
public:
	// Constructors
	Server();
	~Server();
	Server(const Server& other);
	Server& operator=(const Server& other);

	// Getters
	const std::vector<int>& getPorts() const;
	const std::string& getHost() const;
	const std::vector<std::string>& getServerNames() const;
	size_t getMaxBodySize() const;
	const std::map<int, std::string>& getErrorPages() const;
	const std::vector<Route>& getRoutes() const;
	bool isDefaultServer() const;

	// Setters
	void addPort(int port);
	void setHost(const std::string& host);
	void addServerName(const std::string& serverName);
	void setMaxBodySize(size_t size);
	void setErrorPage(int code, const std::string& path);
	void addRoute(const Route& route);
	void setDefaultServer(bool isDefault);

	// Route matching
	const Route* matchRoute(const std::string& path) const;

	// Error page retrieval
	std::string getErrorPage(int code) const;

	// Validation
	bool isValid() const;

	// Debug
	void print() const;

private:
	std::vector<int> _ports;                    // Portas onde o server escuta
	std::string _host;                          // Host (ex: localhost, 0.0.0.0)
	std::vector<std::string> _serverNames;      // Server names (ex: example.com, www.example.com)
	size_t _maxBodySize;                        // Tamanho máximo do body (bytes)
	std::map<int, std::string> _errorPages;     // Error pages customizadas
	std::vector<Route> _routes;                 // Routes/locations
	bool _isDefaultServer;                      // É o default server para este host:port?
};
