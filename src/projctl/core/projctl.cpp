#include "projctl.hpp"

#include "config.hpp"
#include "systeminterface.hpp"

#include <cstdlib>
#include <unistd.h>

ProjCtl::ProjCtl()
	: config_path{SystemInterface::config_home() / "projctl/config.ini"},
	  data_path{SystemInterface::data_home() / "projctl/projects.ini"},
	  config{load_config(config_path).value_or(Config{})},
	  use_colors{config.colors.enabled && std::getenv("NO_COLOR") == nullptr && isatty(STDOUT_FILENO) != 0} {
	load_projects();
}
