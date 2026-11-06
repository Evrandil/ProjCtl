#include "projctl.hpp"

#include "systeminterface.hpp"
#include <unistd.h>   // isatty, STDOUT_FILENO
#include <cstdlib>    // std::getenv


ProjCtl::ProjCtl()
{
	config_path = SystemInterface::config_home() / "projctl/config.ini";
	data_path   = SystemInterface::data_home() / "projctl/projects.ini";
	load();
	use_colors = config.colors_enabled && std::getenv("NO_COLOR") == nullptr && isatty(STDOUT_FILENO) != 0; // NOLINT(cppcoreguidelines-prefer-member-initializer) - need to be after config is loaded
}
