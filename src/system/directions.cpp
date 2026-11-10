#include "systeminterface.hpp"

#include <cstdlib>
#include <filesystem>

auto SystemInterface::config_home() -> std::filesystem::path {
	const char* xdg = std::getenv("XDG_CONFIG_HOME");

	if (xdg && *xdg != '\0') {
		return std::filesystem::path{xdg};
	}

	const char* home = std::getenv("HOME");

	if (home && *home != '\0') {
		return std::filesystem::path{home} / ".config";
	}

	return {};
}

auto SystemInterface::data_home() -> std::filesystem::path {
	const char* xdg = std::getenv("XDG_DATA_HOME");

	if (xdg && *xdg != '\0') {
		return std::filesystem::path{xdg};
	}

	const char* home = std::getenv("HOME");

	if (home && *home != '\0') {
		return std::filesystem::path{home} / ".local/share";
	}

	return {};
}

auto SystemInterface::cache_home() -> std::filesystem::path {
	const char* xdg = std::getenv("XDG_CACHE_HOME");

	if (xdg && *xdg != '\0') {
		return std::filesystem::path{xdg};
	}

	const char* home = std::getenv("HOME");

	if (home && *home != '\0') {
		return std::filesystem::path{home} / ".cache";
	}

	return {};
}
