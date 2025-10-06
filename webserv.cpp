#include "includes/webserv.hpp"

// TODO: Implementar as classes Settings, Instance, HTTP::ServerManager, Utils
// Por enquanto, comentando para permitir compilação

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;

	std::cout << "Webserv iniciando..." << std::endl; //[DEBUG]

	// Initialize settings inside main to avoid static initialization issues
	Settings* settings = Instance::Get<Settings>();

	// TODO: Implementar lógica do servidor

	ac--;
	av++;
	if (!settings->isValid())
		return 1;

	//YAML::RunTests();
	HTTP::ServerManager* serverManager = Instance::Get<HTTP::ServerManager>();

	try {
		// Simplified: just use a default config file path for now
		std::string configFile = (ac > 0) ? av[0] : "config/default.conf";
		if (!serverManager->loadConfig(configFile))
			return 1;
	}
	catch (const std::exception& e) {
		Utils::showException("Failed to load configuration file", e);
		return 1;
	}


	return 0;
}
