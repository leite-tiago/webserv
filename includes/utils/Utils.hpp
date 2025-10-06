/**
 * Utils.hpp
 * Utility functions and helper methods
 */
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <exception>

class Utils {
public:
    /**
     * Split a string by delimiter
     * @param str: String to split
     * @param delimiter: Delimiter to split by
     * @return: Vector of string parts
     */
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);

    /**
     * Check if a string represents an integer
     * @param str: String to check
     * @param allowNegative: Whether to allow negative numbers
     * @return: true if string is a valid integer
     */
    static bool isInteger(const std::string& str, bool allowNegative = false);

    /**
     * Show exception with formatted error message
     * @param message: Context message
     * @param e: Exception to display
     */
    static void showException(const std::string& message, const std::exception& e);
};
