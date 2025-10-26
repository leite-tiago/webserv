/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tborges- <tborges-@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 17:40:13 by tborges-          #+#    #+#             */
/*   Updated: 2025/10/26 17:40:14 by tborges-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// C++ standard library includes
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <cstdlib>

// webserv manual includes
#include "includes/utils/Logger.hpp"
#include "includes/core/Instance.hpp"
#include "includes/core/Settings.hpp"
#include "includes/config/Config.hpp"
#include "includes/config/Server.hpp"
#include "includes/config/Route.hpp"
#include "includes/config/ConfigParser.hpp"
#include "includes/network/Socket.hpp"
#include "includes/network/Connection.hpp"
#include "includes/http/ServerManager.hpp"
#include "includes/cgi/CGIExecutor.hpp"
