/**
 * Socket.cpp
 * Implementation of Socket class
 */
#include "includes/Socket.hpp"
#include "includes/utils/Logger.hpp"
#include <cstring>
#include <cerrno>
#include <sstream>

// Constructors
Socket::Socket()
	: _fd(-1)
	, _host("")
	, _port(0)
	, _valid(false) {
}

Socket::Socket(int fd)
	: _fd(fd)
	, _host("")
	, _port(0)
	, _valid(fd >= 0) {
}

Socket::~Socket() {
	close();
}

// Socket operations
bool Socket::create() {
	// Create TCP socket
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		Logger::error << "Failed to create socket: " << std::strerror(errno) << std::endl;
		_valid = false;
		return false;
	}

	_valid = true;
	Logger::debug << "Socket created with fd: " << _fd << std::endl;
	return true;
}

bool Socket::bind(const std::string& host, int port) {
	if (!_valid) {
		Logger::error << "Cannot bind invalid socket" << std::endl;
		return false;
	}

	_host = host;
	_port = port;

	// Setup address structure
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	// Convert host string to address
	if (host == "0.0.0.0" || host.empty()) {
		addr.sin_addr.s_addr = INADDR_ANY;
	} else {
		if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
			Logger::error << "Invalid host address: " << host << std::endl;
			return false;
		}
	}

	// Bind socket
	if (::bind(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		Logger::error << "Failed to bind socket to " << host << ":" << port
		              << " - " << std::strerror(errno) << std::endl;
		return false;
	}

	Logger::info << "Socket bound to " << host << ":" << port << std::endl;
	return true;
}

bool Socket::listen(int backlog) {
	if (!_valid) {
		Logger::error << "Cannot listen on invalid socket" << std::endl;
		return false;
	}

	if (::listen(_fd, backlog) < 0) {
		Logger::error << "Failed to listen on socket: " << std::strerror(errno) << std::endl;
		return false;
	}

	Logger::info << "Socket listening with backlog: " << backlog << std::endl;
	return true;
}

int Socket::accept(struct sockaddr_in& clientAddr) {
	if (!_valid) {
		Logger::error << "Cannot accept on invalid socket" << std::endl;
		return -1;
	}

	socklen_t addrLen = sizeof(clientAddr);
	std::memset(&clientAddr, 0, sizeof(clientAddr));

	int clientFd = ::accept(_fd, (struct sockaddr*)&clientAddr, &addrLen);
	if (clientFd < 0) {
		// EWOULDBLOCK/EAGAIN is not an error in non-blocking mode
		if (errno != EWOULDBLOCK && errno != EAGAIN) {
			Logger::error << "Failed to accept connection: " << std::strerror(errno) << std::endl;
		}
		return -1;
	}

	Logger::debug << "Accepted connection from " << getHostString(clientAddr)
	              << ":" << getPortNumber(clientAddr) << " (fd: " << clientFd << ")" << std::endl;
	return clientFd;
}

void Socket::close() {
	if (_valid && _fd >= 0) {
		::close(_fd);
		Logger::debug << "Socket closed (fd: " << _fd << ")" << std::endl;
		_fd = -1;
		_valid = false;
	}
}

// Configuration
bool Socket::setNonBlocking() {
	if (!_valid) {
		Logger::error << "Cannot set non-blocking on invalid socket" << std::endl;
		return false;
	}

	// Get current flags
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags < 0) {
		Logger::error << "Failed to get socket flags: " << std::strerror(errno) << std::endl;
		return false;
	}

	// Set non-blocking flag
	if (fcntl(_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		Logger::error << "Failed to set non-blocking: " << std::strerror(errno) << std::endl;
		return false;
	}

	Logger::debug << "Socket set to non-blocking mode (fd: " << _fd << ")" << std::endl;
	return true;
}

bool Socket::setReuseAddr() {
	if (!_valid) {
		Logger::error << "Cannot set SO_REUSEADDR on invalid socket" << std::endl;
		return false;
	}

	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		Logger::error << "Failed to set SO_REUSEADDR: " << std::strerror(errno) << std::endl;
		return false;
	}

	Logger::debug << "Socket SO_REUSEADDR enabled (fd: " << _fd << ")" << std::endl;
	return true;
}

bool Socket::setReusePort() {
	if (!_valid) {
		Logger::error << "Cannot set SO_REUSEPORT on invalid socket" << std::endl;
		return false;
	}

	// SO_REUSEPORT may not be available on all systems
#ifdef SO_REUSEPORT
	int opt = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
		Logger::warning << "Failed to set SO_REUSEPORT: " << std::strerror(errno) << std::endl;
		return false;
	}
	Logger::debug << "Socket SO_REUSEPORT enabled (fd: " << _fd << ")" << std::endl;
#else
	Logger::debug << "SO_REUSEPORT not available on this system" << std::endl;
#endif

	return true;
}

// Getters
int Socket::getFd() const {
	return _fd;
}

const std::string& Socket::getHost() const {
	return _host;
}

int Socket::getPort() const {
	return _port;
}

bool Socket::isValid() const {
	return _valid;
}

// Utils
std::string Socket::getHostString(const struct sockaddr_in& addr) {
	char buffer[INET_ADDRSTRLEN];
	if (inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN) == NULL) {
		return "unknown";
	}
	return std::string(buffer);
}

int Socket::getPortNumber(const struct sockaddr_in& addr) {
	return ntohs(addr.sin_port);
}
