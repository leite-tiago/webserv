/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:38:28 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:29 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Logger.cpp
 * Implementation of Logger utility
 */
#include "includes/utils/Logger.hpp"
#include <ctime>

namespace Logger {

    // Stream constructor with custom output stream
    Stream::Stream(std::ostream& out, const std::string& header, const std::string& color, bool enabled)
        : header(header), out(out), color(color), enabled(enabled) {}

    // Stream constructor with default std::cout
    Stream::Stream(const std::string& header, const std::string& color, bool enabled)
        : header(header), out(std::cout), color(color), enabled(enabled) {}

    // Destructor
    Stream::~Stream() {}

    // Copy constructor
    Stream::Stream(const Stream& other)
        : header(other.header), out(other.out), color(other.color), enabled(other.enabled) {}

    // Assignment operator
    Stream& Stream::operator=(const Stream& other) {
        if (this != &other) {
            header = other.header;
            color = other.color;
            enabled = other.enabled;
            // Note: out is a reference and cannot be reassigned
        }
        return *this;
    }

    // Build header with timestamp and formatting
    std::string Stream::buildHeader() const {
        return color + "[" + getTime() + "] " + header + RESET;
    }

    // Get current timestamp
    std::string Stream::getTime() const {
        time_t now = time(0);
        char* timeStr = ctime(&now);
        std::string result(timeStr);
        // Remove trailing newline
        if (!result.empty() && result[result.length() - 1] == '\n') {
            result.erase(result.length() - 1);
        }
        return result;
    }

    // Pre-defined logger instances
    Stream info(std::cout, "INFO", BLUE, true);
    Stream debug(std::cout, "DEBUG", DARK_GRAY, true);
    Stream warning(std::cout, "WARNING", DARK_YELLOW, true);
    Stream error(std::cerr, "ERROR", RED, true);
    Stream success(std::cout, "SUCCESS", GREEN, true);
    Stream child(std::cout, "CHILD", CYAN, true);
}
