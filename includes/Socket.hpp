/**
 * Socket.hpp
 * Wrapper class for socket operations
 * Handles socket creation, binding, listening, and configuration
 */
#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class Socket {
public:
	// Constructors
	Socket();
	Socket(int fd);
	~Socket();

	// Socket operations
	bool create();
	bool bind(const std::string& host, int port);
	bool listen(int backlog = 128);
	int accept(struct sockaddr_in& clientAddr);
	void close();

	// Configuration
	bool setNonBlocking();
	bool setReuseAddr();
	bool setReusePort();

	// Getters
	int getFd() const;
	const std::string& getHost() const;
	int getPort() const;
	bool isValid() const;

	// Utils
	static std::string getHostString(const struct sockaddr_in& addr);
	static int getPortNumber(const struct sockaddr_in& addr);

private:
	int _fd;           // File descriptor
	std::string _host; // Host address (e.g., "127.0.0.1", "0.0.0.0")
	int _port;         // Port number
	bool _valid;       // Is socket valid?

	// Disable copy (C++98 way)
	Socket(const Socket& other);
	Socket& operator=(const Socket& other);
};
