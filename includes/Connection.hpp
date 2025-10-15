/**
 * Connection.hpp
 * Represents a client connection with state management
 * Handles reading requests and writing responses
 */
#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http/Request.hpp"
#include "http/Response.hpp"

// Forward declarations
class Server;

class Connection {
public:
	// Connection states
	enum State {
		READING_REQUEST,   // Reading HTTP request from client
		PROCESSING,        // Processing request (e.g., CGI)
		WRITING_RESPONSE,  // Writing HTTP response to client
		CLOSING            // Connection should be closed
	};

	// Constructors
	Connection(int fd, const struct sockaddr_in& addr, const Server* server);
	~Connection();

	// I/O operations
	bool readRequest();
	bool writeResponse();

	// State management
	State getState() const;
	void setState(State state);

	// Getters
	int getFd() const;
	const std::string& getClientHost() const;
	int getClientPort() const;
	time_t getLastActivity() const;
	bool shouldClose() const;

	// Timeout check
	bool isTimedOut(time_t timeout) const;

	// Buffer management
	const std::string& getRequestBuffer() const;
	void clearRequestBuffer();

private:
	int _fd;                      // Client socket file descriptor
	struct sockaddr_in _addr;     // Client address
	std::string _clientHost;      // Client host string
	int _clientPort;              // Client port
	const Server* _server;        // Associated server configuration

	State _state;                 // Current connection state
	time_t _lastActivity;         // Last activity timestamp

	std::string _requestBuffer;   // Buffer for incoming request
	std::string _responseBuffer;  // Buffer for outgoing response
	size_t _responseOffset;       // Offset for partial writes

	bool _keepAlive;              // Keep-alive connection?
	bool _shouldClose;            // Should close after response?

	// Disable copy
	Connection(const Connection& other);
	Connection& operator=(const Connection& other);

	// Helper methods
	void updateActivity();
};
