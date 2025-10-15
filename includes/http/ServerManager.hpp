/**
 * ServerManager.hpp
 * HTTP Server Manager class for handling web server operations
 * Manages multiple listening sockets and client connections using poll()
 */
#pragma once

#include "includes/config/Config.hpp"
#include "includes/network/Socket.hpp"
#include "includes/network/Connection.hpp"
#include <string>
#include <vector>
#include <map>
#include <poll.h>

namespace HTTP {
	class ServerManager {
	public:
		/**
		 * Constructor
		 */
		ServerManager();

		/**
		 * Destructor
		 */
		~ServerManager();

		/**
		 * Initialize with configuration
		 * @param config: Parsed configuration
		 * @return: true if initialized successfully
		 */
		bool init(const Config& config);

		/**
		 * Start the server (blocking loop)
		 * @return: true if server stopped normally
		 */
		bool run();

		/**
		 * Stop the server
		 */
		void stop();

		/**
		 * Check if server is running
		 */
		bool isRunning() const;

	private:
		Config _config;                           // Server configuration
		std::vector<Socket*> _listeningSockets;   // Listening sockets
		std::map<int, Connection*> _connections;  // Active connections (fd -> Connection)
		std::vector<struct pollfd> _pollFds;      // Poll file descriptors
		bool _running;                            // Is server running?
		time_t _timeout;                          // Connection timeout (seconds)

		// Setup
		bool setupListeningSockets();
		Socket* createListeningSocket(const std::string& host, int port);

		// Poll management
		void rebuildPollFds();
		void addToPoll(int fd, short events);
		void removeFromPoll(int fd);

		// Event handling
		void handleListeningSocket(int fd);
		void handleClientSocket(int fd, short revents);
		void acceptNewConnection(Socket* listenSocket);
		void closeConnection(int fd);

		// Cleanup
		void cleanupTimedOutConnections();
		void cleanupAllConnections();

		// Disable copy
		ServerManager(const ServerManager& other);
		ServerManager& operator=(const ServerManager& other);
	};
}
