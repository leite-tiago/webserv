#include "includes/webserv.hpp"

void Utils::showException(const std::string& message, const std::exception& e)
{
	std::stringstream ss;
	ss << message << ": " << Logger::param(e.what());
	if (errno != 0)
		ss << " (" << Logger::param(strerror(errno)) << ")";
	ss << std::newl;
	Logger::error(ss.str());
}
