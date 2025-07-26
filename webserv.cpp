#include "includes/webserv.hpp"

static Settings* settings = Instance::Get<Settings>();

int	main(int ac, char **av, char **env)
{
	ac--;
	av++;
	(void)env;
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
}
