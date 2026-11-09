#include "projctl.hpp"

#include "systeminterface.hpp"
#include <cstdlib>  // std::getenv
#include <unistd.h> // isatty, STDOUT_FILENO

ProjCtl::ProjCtl() {
	config_path = SystemInterface::config_home() / "projctl/config.ini";
	data_path   = SystemInterface::data_home() / "projctl/projects.ini";
	load();
	use_colors = config.colors.enabled && // NOLINT(cppcoreguidelines-prefer-member-initializer) - need to be after
	                                      // config is loaded
	             std::getenv("NO_COLOR") == nullptr && isatty(STDOUT_FILENO) != 0;
}
