/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:38:43 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:44 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * ServerManager.cpp
 * Implementation of HTTP Server Manager
 */
#include "includes/http/ServerManager.hpp"
#include "includes/utils/Logger.hpp"
#include <cstring>
#include <cerrno>
#include <algorithm>
#include <sstream>
#include <signal.h>
#include <fcntl.h>

namespace HTTP {

// Constructor
ServerManager::ServerManager()
	: _running(false)
	, _timeout(60) {
	// Ignore SIGPIPE (broken pipe) - we'll handle write errors instead
	signal(SIGPIPE, SIG_IGN);
}

// Destructor
ServerManager::~ServerManager() {
	cleanupAllConnections();

	// Close listening sockets
	for (size_t i = 0; i < _listeningSockets.size(); ++i) {
		delete _listeningSockets[i];
	}
	_listeningSockets.clear();
}

// Initialize with configuration
bool ServerManager::init(const Config& config) {
	Logger::info << "Initializing server manager..." << std::endl;

	_config = config;

	if (!setupListeningSockets()) {
		Logger::error << "Failed to setup listening sockets" << std::endl;
		return false;
	}

	Logger::success << "Server manager initialized successfully!" << std::endl;
	return true;
}

// Setup listening sockets
bool ServerManager::setupListeningSockets() {
	const std::vector<Server>& servers = _config.getServers();

	// Create listening sockets for each unique host:port combination
	std::map<std::string, bool> uniqueBindings;

	for (size_t i = 0; i < servers.size(); ++i) {
		const Server& server = servers[i];
		const std::string& host = server.getHost();
		const std::vector<int>& ports = server.getPorts();

		for (size_t j = 0; j < ports.size(); ++j) {
			int port = ports[j];

			// Create unique key for this binding
			std::ostringstream key;
			key << host << ":" << port;
			std::string bindingKey = key.str();

			// Skip if we already have a socket for this binding
			if (uniqueBindings.find(bindingKey) != uniqueBindings.end()) {
				Logger::debug << "Socket already exists for " << bindingKey << std::endl;
				continue;
			}

			// Create listening socket
			Socket* sock = createListeningSocket(host, port);
			if (!sock) {
				Logger::error << "Failed to create listening socket for " << host << ":" << port << std::endl;
				return false;
			}

			_listeningSockets.push_back(sock);
			uniqueBindings[bindingKey] = true;

			Logger::success << "Listening on " << host << ":" << port << std::endl;
		}
	}

	if (_listeningSockets.empty()) {
		Logger::error << "No listening sockets created!" << std::endl;
		return false;
	}

	return true;
}

// Create listening socket
Socket* ServerManager::createListeningSocket(const std::string& host, int port) {
	Socket* sock = new Socket();

	// Create socket
	if (!sock->create()) {
		delete sock;
		return NULL;
	}

	// Configure socket
	if (!sock->setReuseAddr()) {
		Logger::warning << "Failed to set SO_REUSEADDR" << std::endl;
	}

	sock->setReusePort(); // May not be available on all systems

	// Bind
	if (!sock->bind(host, port)) {
		delete sock;
		return NULL;
	}

	// Set non-blocking
	if (!sock->setNonBlocking()) {
		delete sock;
		return NULL;
	}

	// Listen
	if (!sock->listen(128)) {
		delete sock;
		return NULL;
	}

	return sock;
}

// Start the server (blocking loop)
bool ServerManager::run() {
	Logger::info << "Starting server..." << std::endl;
	_running = true;

	// Build initial poll fds
	rebuildPollFds();

	Logger::success << "Server running! Press Ctrl+C to stop." << std::endl;
	std::cout << std::endl;
	Logger::info << "Listening on:" << std::endl;
	for (size_t i = 0; i < _listeningSockets.size(); ++i) {
		std::string host = _listeningSockets[i]->getHost();
		int port = _listeningSockets[i]->getPort();

		// Convert 0.0.0.0 to localhost for display
		std::string displayHost = (host == "0.0.0.0") ? "localhost" : host;

		// Build URL
		std::ostringstream url;
		url << "http://" << displayHost << ":" << port;

		std::cout << "  → " << Logger::param(url.str()) << std::endl;
	}
	std::cout << std::endl;

	// Main event loop
	while (_running) {
		// Poll with 1 second timeout
		int pollResult = poll(&_pollFds[0], _pollFds.size(), 1000);

		if (pollResult < 0) {
			if (errno == EINTR) {
				// Interrupted by signal, continue
				continue;
			}
			Logger::error << "poll() failed: " << std::strerror(errno) << std::endl;
			break;
		}

		if (pollResult == 0) {
			// Timeout - check for timed out connections
			cleanupTimedOutConnections();
			continue;
		}

		// Check which file descriptors have events
		for (size_t i = 0; i < _pollFds.size() && pollResult > 0; ++i) {
			struct pollfd& pfd = _pollFds[i];

			if (pfd.revents == 0) {
				continue; // No events for this fd
			}

			--pollResult; // Count down events processed

			// Check if this is a listening socket
			bool isListening = false;
			for (size_t j = 0; j < _listeningSockets.size(); ++j) {
				if (_listeningSockets[j]->getFd() == pfd.fd) {
					isListening = true;
					handleListeningSocket(pfd.fd);
					break;
				}
			}

			// If not listening socket, it's a client connection
			if (!isListening) {
				handleClientSocket(pfd.fd, pfd.revents);
			}
		}

		// Rebuild poll fds if connections changed
		// (we could optimize this to only rebuild when needed)
		rebuildPollFds();
	}

	Logger::info << "Server stopped." << std::endl;
	return true;
}

// Stop the server
void ServerManager::stop() {
	Logger::info << "Stopping server..." << std::endl;
	_running = false;
}

// Check if server is running
bool ServerManager::isRunning() const {
	return _running;
}

// Rebuild poll fds array
void ServerManager::rebuildPollFds() {
	_pollFds.clear();

	// Add listening sockets (monitor for POLLIN - new connections)
	for (size_t i = 0; i < _listeningSockets.size(); ++i) {
		struct pollfd pfd;
		pfd.fd = _listeningSockets[i]->getFd();
		pfd.events = POLLIN;
		pfd.revents = 0;
		_pollFds.push_back(pfd);
	}

	// Add client connections
	for (std::map<int, Connection*>::iterator it = _connections.begin();
	     it != _connections.end(); ++it) {
		Connection* conn = it->second;
		struct pollfd pfd;
		pfd.fd = conn->getFd();
		pfd.revents = 0;

		// Monitor based on connection state
		if (conn->getState() == Connection::READING_REQUEST) {
			pfd.events = POLLIN;  // Monitor for read
		} else if (conn->getState() == Connection::WRITING_RESPONSE) {
			pfd.events = POLLOUT; // Monitor for write
		} else {
			pfd.events = POLLIN | POLLOUT; // Monitor both
		}

		_pollFds.push_back(pfd);
	}
}

// Handle listening socket (new connection)
void ServerManager::handleListeningSocket(int fd) {
	// Find the listening socket
	Socket* listenSocket = NULL;
	for (size_t i = 0; i < _listeningSockets.size(); ++i) {
		if (_listeningSockets[i]->getFd() == fd) {
			listenSocket = _listeningSockets[i];
			break;
		}
	}

	if (!listenSocket) {
		Logger::error << "Listening socket not found for fd: " << fd << std::endl;
		return;
	}

	// Accept new connections (may be multiple)
	while (true) {
		struct sockaddr_in clientAddr;
		int clientFd = listenSocket->accept(clientAddr);

		if (clientFd < 0) {
			break; // No more connections to accept
		}

		// Set client socket to non-blocking
		int flags = fcntl(clientFd, F_GETFL, 0);
		if (flags >= 0) {
			fcntl(clientFd, F_SETFL, flags | O_NONBLOCK);
		}

		Logger::debug << "Socket set to non-blocking mode (fd: " << clientFd << ")" << std::endl;

		// Find the server configuration for this socket
		const Server* server = _config.getDefaultServer(
			listenSocket->getHost(),
			listenSocket->getPort()
		);

		if (!server) {
			Logger::warning << "No server configuration found for connection" << std::endl;
			close(clientFd);
			continue;
		}

		// Create connection object
		Connection* conn = new Connection(clientFd, clientAddr, server);
		_connections[clientFd] = conn;

		Logger::info << "Accepted new connection (fd: " << clientFd
		             << "), total connections: " << _connections.size() << std::endl;
	}
}

// Handle client socket events
void ServerManager::handleClientSocket(int fd, short revents) {
	// Find connection
	std::map<int, Connection*>::iterator it = _connections.find(fd);
	if (it == _connections.end()) {
		Logger::warning << "Connection not found for fd: " << fd << std::endl;
		return;
	}

	Connection* conn = it->second;

	// Check for errors
	if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
		Logger::debug << "Connection error/hangup (fd: " << fd << ")" << std::endl;
		closeConnection(fd);
		return;
	}

	// Handle reading
	if (revents & POLLIN) {
		if (!conn->readRequest()) {
			Logger::debug << "Error reading request (fd: " << fd << ")" << std::endl;
			closeConnection(fd);
			return;
		}
	}

	// Handle writing
	if (revents & POLLOUT) {
		if (conn->getState() == Connection::WRITING_RESPONSE) {
			if (!conn->writeResponse()) {
				Logger::debug << "Error writing response (fd: " << fd << ")" << std::endl;
				closeConnection(fd);
				return;
			}
		}
	}

	// Check if connection should be closed
	if (conn->shouldClose()) {
		closeConnection(fd);
	}
}

// Close connection
void ServerManager::closeConnection(int fd) {
	std::map<int, Connection*>::iterator it = _connections.find(fd);
	if (it != _connections.end()) {
		Logger::debug << "Closing connection (fd: " << fd << ")" << std::endl;
		delete it->second;
		_connections.erase(it);
	}
}

// Cleanup timed out connections
void ServerManager::cleanupTimedOutConnections() {
	std::vector<int> toClose;

	for (std::map<int, Connection*>::iterator it = _connections.begin();
	     it != _connections.end(); ++it) {
		if (it->second->isTimedOut(_timeout)) {
			Logger::warning << "Connection timed out (fd: " << it->first << ")" << std::endl;
			toClose.push_back(it->first);
		}
	}

	for (size_t i = 0; i < toClose.size(); ++i) {
		closeConnection(toClose[i]);
	}
}

// Cleanup all connections
void ServerManager::cleanupAllConnections() {
	for (std::map<int, Connection*>::iterator it = _connections.begin();
	     it != _connections.end(); ++it) {
		delete it->second;
	}
	_connections.clear();
}

} // namespace HTTP
