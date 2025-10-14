#include "includes/webserv.hpp"

int	main(int ac, char **av, char **env)
{
	(void)env;

	std::cout << "Webserv starting..." << std::endl;

	// Initialize settings
	Settings* settings = Instance::Get<Settings>();
	if (!settings->isValid()) {
		std::cerr << "Error: Failed to load settings" << std::endl;
		return 1;
	}

	// Determine config file path
	std::string configFile = (ac > 1) ? av[1] : "config/default.conf";
	std::cout << "Loading configuration from: " << configFile << std::endl;

	// Parse configuration
	Config config;
	ConfigParser parser;

	if (!parser.parse(configFile, config)) {
		std::cerr << "Error: " << parser.getError() << std::endl;
		return 1;
	}

	std::cout << "Configuration loaded successfully!" << std::endl;
	std::cout << std::endl;

	// Print configuration (debug)
	config.print();

	// TODO: Start server manager with the parsed configuration
	// HTTP::ServerManager* serverManager = Instance::Get<HTTP::ServerManager>();
	// serverManager->start(config);

	std::cout << std::endl;
	std::cout << "Press Ctrl+C to stop the server..." << std::endl;

	return 0;
}
