#include "includes/webserv.hpp"
#include <cstring>
#include <cctype>

std::vector<std::string> Utils::split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        if (end != start) {
            parts.push_back(str.substr(start, end - start));
        }
        start = end + delimiter.length();
    }

    if (start < str.length()) {
        parts.push_back(str.substr(start));
    }

    return parts;
}

bool Utils::isInteger(const std::string& str, bool allowNegative)
{
    if (str.empty()) return false;

    size_t start = 0;
    if (allowNegative && str[0] == '-') {
        if (str.length() == 1) return false;
        start = 1;
    }

    for (size_t i = start; i < str.length(); ++i) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }

    return true;
}

void Utils::showException(const std::string& message, const std::exception& e)
{
	std::stringstream ss;
	ss << message << ": " << Logger::param(e.what());
	if (errno != 0)
		ss << " (" << Logger::param(strerror(errno)) << ")";
	ss << std::endl;
	Logger::error << ss.str() << std::endl;
}
