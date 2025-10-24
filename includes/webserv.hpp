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
