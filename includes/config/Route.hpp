/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:40:41 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:40:42 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Route.hpp
 * Representa uma route/location de um servidor HTTP
 * Configurações específicas para um caminho (ex: /uploads, /cgi-bin)
 */
#pragma once

#include <string>
#include <vector>
#include <map>

class Route {
public:
	// Constructors
	Route();
	Route(const std::string& path);
	~Route();
	Route(const Route& other);
	Route& operator=(const Route& other);

	// Getters
	const std::string& getPath() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::string& getRedirect() const;
	const std::string& getRoot() const;
	bool isDirectoryListingEnabled() const;
	const std::vector<std::string>& getIndexFiles() const;
	bool isCgiEnabled() const;
	const std::string& getCgiPath() const;
	const std::string& getCgiExtension() const;
	bool isUploadEnabled() const;
	const std::string& getUploadPath() const;

	// Setters
	void setPath(const std::string& path);
	void addAllowedMethod(const std::string& method);
	void setRedirect(const std::string& redirect);
	void setRoot(const std::string& root);
	void setDirectoryListing(bool enabled);
	void addIndexFile(const std::string& indexFile);
	void setCgiEnabled(bool enabled);
	void setCgiPath(const std::string& cgiPath);
	void setCgiExtension(const std::string& extension);
	void setUploadEnabled(bool enabled);
	void setUploadPath(const std::string& uploadPath);

	// Validation
	bool isMethodAllowed(const std::string& method) const;
	bool isValid() const;

	// Debug
	void print() const;

private:
	std::string _path;                          // Path da route (ex: /uploads)
	std::vector<std::string> _allowedMethods;   // Métodos permitidos (GET, POST, DELETE)
	std::string _redirect;                      // Redirect URL (se configurado)
	std::string _root;                          // Root directory para esta route
	bool _directoryListing;                     // Directory listing enabled?
	std::vector<std::string> _indexFiles;       // Index files (index.html, index.php)
	bool _cgiEnabled;                           // CGI enabled para esta route?
	std::string _cgiPath;                       // Path do executável CGI
	std::string _cgiExtension;                  // Extensão de ficheiros CGI (.php, .py)
	bool _uploadEnabled;                        // Upload enabled?
	std::string _uploadPath;                    // Directory para uploads
};
