#include "includes/webserv.hpp"
#include <csignal>

// Global pointer to server manager for signal handling
HTTP::ServerManager* g_serverManager = NULL;

void signalHandler(int signal) {
	(void)signal;
	std::cout << std::endl;
	Logger::warning << "Received interrupt signal, stopping server..." << std::endl;
	if (g_serverManager) {
		g_serverManager->stop();
	}
}

int	main(int ac, char **av, char **env)
{
	(void)env;

	std::cout << "╔══════════════════════════════════════╗" << std::endl;
	std::cout << "║          WEBSERV HTTP/1.1            ║" << std::endl;
	std::cout << "║         Starting server...           ║" << std::endl;
	std::cout << "╚══════════════════════════════════════╝" << std::endl;
	std::cout << std::endl;

	// Initialize settings
	Settings* settings = Instance::Get<Settings>();
	if (!settings->isValid()) {
		Logger::error << "Failed to load settings" << std::endl;
		return 1;
	}

	// Determine config file path
	std::string configFile = (ac > 1) ? av[1] : "config/default.conf";
	Logger::info << "Loading configuration from: " << Logger::param(configFile) << std::endl;

	// Parse configuration
	Config config;
	ConfigParser parser;
	
	if (!parser.parse(configFile, config)) {
		Logger::error << parser.getError() << std::endl;
		return 1;
	}

	Logger::success << "Configuration loaded successfully!" << std::endl;
	std::cout << std::endl;

	// Initialize server manager
	HTTP::ServerManager serverManager;
	g_serverManager = &serverManager;
	
	if (!serverManager.init(config)) {
		Logger::error << "Failed to initialize server manager" << std::endl;
		return 1;
	}

	// Setup signal handlers
	signal(SIGINT, signalHandler);  // Ctrl+C
	signal(SIGTERM, signalHandler); // kill

	std::cout << std::endl;

	// Start server (blocking)
	serverManager.run();

	std::cout << std::endl;
	Logger::success << "Server shutdown complete." << std::endl;

	return 0;
}