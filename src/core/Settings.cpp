/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Settings.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:38:49 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:38:50 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Settings.cpp
 * Implementation of Settings class
 */
#include "includes/core/Settings.hpp"

Settings::Settings() {
    // Constructor - settings are hard-coded
}

bool Settings::isValid() const {
    // Always valid (hard-coded settings)
    return true;
}

const std::string Settings::httpStatusCode(int code) const {
    // Basic HTTP status code mapping
    // In a real implementation, this would be loaded from config
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 408: return "Request Timeout";
        case 413: return "Payload Too Large";
        case 414: return "URI Too Long";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        case 505: return "HTTP Version Not Supported";
        default: return "Unknown Status";
    }
}

const std::string& Settings::httpMimeType(const std::string& ext) const {
    // Static mime type mapping
    // In a real implementation, this would be loaded from config
    static std::map<std::string, std::string> mimeTypes;
    static bool initialized = false;

    // Initialize the map on first use (C++98 compatible)
    if (!initialized) {
        mimeTypes["html"] = "text/html";
        mimeTypes["htm"] = "text/html";
        mimeTypes["css"] = "text/css";
        mimeTypes["js"] = "application/javascript";
        mimeTypes["json"] = "application/json";
        mimeTypes["xml"] = "application/xml";
        mimeTypes["txt"] = "text/plain";
        mimeTypes["csv"] = "text/csv";
        mimeTypes["png"] = "image/png";
        mimeTypes["jpg"] = "image/jpeg";
        mimeTypes["jpeg"] = "image/jpeg";
        mimeTypes["gif"] = "image/gif";
        mimeTypes["svg"] = "image/svg+xml";
        mimeTypes["ico"] = "image/x-icon";
        mimeTypes["pdf"] = "application/pdf";
        mimeTypes["zip"] = "application/zip";
        mimeTypes["tar"] = "application/x-tar";
        mimeTypes["gz"] = "application/gzip";
        initialized = true;
    }

    static std::string defaultType = "application/octet-stream";

    std::map<std::string, std::string>::const_iterator it = mimeTypes.find(ext);
    if (it != mimeTypes.end()) {
        return it->second;
    }

    return defaultType;
}
