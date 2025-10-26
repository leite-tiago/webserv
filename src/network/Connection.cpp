/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:38:33 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:34 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Connection.cpp
 * Implementation of Connection class
 */
#include "includes/network/Connection.hpp"
#include "includes/network/Socket.hpp"
#include "includes/config/Server.hpp"
#include "includes/http/RequestHandler.hpp"
#include "includes/utils/Logger.hpp"
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <ctime>

// Constructors
Connection::Connection(int fd, const struct sockaddr_in& addr, const Server* server)
	: _fd(fd)
	, _addr(addr)
	, _clientHost(Socket::getHostString(addr))
	, _clientPort(Socket::getPortNumber(addr))
	, _server(server)
	, _state(READING_REQUEST)
	, _lastActivity(std::time(NULL))
	, _responseOffset(0)
	, _keepAlive(false)
	, _shouldClose(false) {

	Logger::info << "New connection from " << _clientHost << ":" << _clientPort
	             << " (fd: " << _fd << ")" << std::endl;
}

Connection::~Connection() {
	if (_fd >= 0) {
		::close(_fd);
		Logger::debug << "Connection closed (fd: " << _fd << ")" << std::endl;
	}
}

// I/O operations
bool Connection::readRequest() {
	const size_t BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];

	ssize_t bytesRead = recv(_fd, buffer, BUFFER_SIZE - 1, 0);

	if (bytesRead < 0) {
		// Non-blocking socket: would block means no data available
		// Don't check errno - just return success and try again later
		return true; // Not an error for non-blocking sockets
	}

	if (bytesRead == 0) {
		// Client closed connection
		Logger::debug << "Client closed connection (fd: " << _fd << ")" << std::endl;
		_shouldClose = true;
		return false;
	}

	// Append to request buffer
	buffer[bytesRead] = '\0';
	_requestBuffer.append(buffer, bytesRead);
	updateActivity();

	Logger::debug << "Read " << bytesRead << " bytes from connection (fd: " << _fd
	              << "), total: " << _requestBuffer.size() << " bytes" << std::endl;

	// Check if we have a complete HTTP request
	if (_requestBuffer.find("\r\n\r\n") != std::string::npos) {
		Logger::debug << "Complete request received (fd: " << _fd << ")" << std::endl;
		_state = PROCESSING;

		// Parse HTTP request
		HTTP::Request request;
		if (!request.parse(_requestBuffer)) {
			Logger::error << "Failed to parse HTTP request" << std::endl;
			HTTP::Response errorResp = HTTP::Response::errorResponse(400, "Bad Request");
			_responseBuffer = errorResp.build();
			_responseOffset = 0;
			_state = WRITING_RESPONSE;
			_shouldClose = true;
			return true;
		}

		// Debug: print request
		if (Logger::debug << "") {
			request.print();
		}

		// Handle request
		HTTP::RequestHandler handler(_server);
		HTTP::Response response = handler.handle(request);

		// Build response
		_responseBuffer = response.build();
		_responseOffset = 0;
		_state = WRITING_RESPONSE;
		// Don't set _shouldClose here - let writeResponse handle it
	}

	return true;
}

bool Connection::writeResponse() {
	if (_responseBuffer.empty() || _responseOffset >= _responseBuffer.size()) {
		// Nothing to write or already written everything
		_shouldClose = true;
		return true;
	}

	const char* data = _responseBuffer.c_str() + _responseOffset;
	size_t remaining = _responseBuffer.size() - _responseOffset;

	ssize_t bytesWritten = send(_fd, data, remaining, 0);

	if (bytesWritten < 0) {
		// Non-blocking socket: would block means socket not ready
		// Don't check errno - just return success and try again later
		return true; // Not an error for non-blocking sockets
	}

	_responseOffset += bytesWritten;
	updateActivity();

	Logger::debug << "Wrote " << bytesWritten << " bytes to connection (fd: " << _fd
	              << "), total: " << _responseOffset << "/" << _responseBuffer.size()
	              << " bytes" << std::endl;

	// Check if response is complete
	if (_responseOffset >= _responseBuffer.size()) {
		Logger::info << "Response complete (fd: " << _fd << ")" << std::endl;
		_shouldClose = true;
		_state = CLOSING;
	}

	return true;
}

// State management
Connection::State Connection::getState() const {
	return _state;
}

void Connection::setState(State state) {
	_state = state;
}

// Getters
int Connection::getFd() const {
	return _fd;
}

const std::string& Connection::getClientHost() const {
	return _clientHost;
}

int Connection::getClientPort() const {
	return _clientPort;
}

time_t Connection::getLastActivity() const {
	return _lastActivity;
}

bool Connection::shouldClose() const {
	return _shouldClose;
}

// Timeout check
bool Connection::isTimedOut(time_t timeout) const {
	return (std::time(NULL) - _lastActivity) > timeout;
}

// Buffer management
const std::string& Connection::getRequestBuffer() const {
	return _requestBuffer;
}

void Connection::clearRequestBuffer() {
	_requestBuffer.clear();
}

// Helper methods
void Connection::updateActivity() {
	_lastActivity = std::time(NULL);
}
