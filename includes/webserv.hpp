#pragma once

// C++ standard library includes
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stack>
#include <stdint.h>
#include <typeinfo>
#include <execinfo.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cerrno>
#include <cstdlib>

// webserv manual includes
#include "Yaml.hpp"
#include "shared.hpp"
#include "utils/Logger.hpp"
#include "utils/Utils.hpp"
#include "Instance.hpp"
#include "Settings.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "ConfigParser.hpp"
#include "Socket.hpp"
#include "Connection.hpp"
#include "http/ServerManager.hpp"
#include "cgi/CGIExecutor.hpp"
