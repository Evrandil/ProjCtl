#include "systeminterface.hpp"


std::filesystem::path SystemInterface::config_home()
{
	const char* xdg = std::getenv("XDG_CONFIG_HOME");

	if (xdg && *xdg != '\0')
	{
		return std::filesystem::path{xdg};
	}

	const char* home = std::getenv("HOME");

	if (home && *home != '\0')
	{
		return std::filesystem::path{home} / ".config";
	}

	return {};
}


std::filesystem::path SystemInterface::data_home()
{
	const char* xdg = std::getenv("XDG_DATA_HOME");

	if (xdg && *xdg != '\0')
	{
		return std::filesystem::path{xdg};
	}

	const char* home = std::getenv("HOME");

	if (home && *home != '\0')
	{
		return std::filesystem::path{home} / ".local/share";
	}

	return {};
}


std::filesystem::path SystemInterface::cache_home()
{
	const char* xdg = std::getenv("XDG_CACHE_HOME");

	if (xdg && *xdg != '\0')
	{
		return std::filesystem::path{xdg};
	}

	const char* home = std::getenv("HOME");

	if (home && *home != '\0')
	{
		return std::filesystem::path{home} / ".cache";
	}

	return {};
}
