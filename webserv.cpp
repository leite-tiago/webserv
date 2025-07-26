#include "includes/webserv.hpp"

// TODO: Implementar as classes Settings, Instance, HTTP::ServerManager, Utils
// Por enquanto, comentando para permitir compilação
static Settings* settings = Instance::Get<Settings>();

int	main(int ac, char **av, char **env)
{
	(void)ac;
	(void)av;
	(void)env;

	std::cout << "Webserv iniciando..." << std::endl; //[DEBUG]

	// TODO: Implementar lógica do servidor

	ac--;
	av++;
	if (!settings->isValid())
		return 1;

	//YAML::RunTests();
	//HTTP::ServerManager* serverManager = Instance::Get<HTTP::ServerManager>();

	try {
		if (!serverManager->loadConfig(ac > 0 ? av[0] : settings->get<std::string>("misc.default_config_file")))
			return 1;
	}
	catch (const std::exception& e) {
		Utils::showException("Failed to load configuration file", e);
		return 1;
	}


	return 0;
}
