/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:40:45 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:40:46 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * Config.hpp
 * Classe principal que armazena toda a configuração do webserv
 * Contém múltiplos servers e métodos para acesso
 */
#pragma once

#include "includes/config/Server.hpp"
#include <string>
#include <vector>

class Config {
public:
	// Constructors
	Config();
	~Config();
	Config(const Config& other);
	Config& operator=(const Config& other);

	// Server management
	void addServer(const Server& server);
	const std::vector<Server>& getServers() const;

	// Server lookup
	const Server* getServer(const std::string& host, int port, const std::string& serverName = "") const;
	const Server* getDefaultServer(const std::string& host, int port) const;

	// Validation
	bool isValid() const;

	// Debug
	void print() const;

private:
	std::vector<Server> _servers;  // Lista de todos os servers configurados
};
