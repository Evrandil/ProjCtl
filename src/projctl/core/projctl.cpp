#include "projctl.hpp"

#include "systeminterface.hpp"


ProjCtl::ProjCtl()
{
	config_path = SystemInterface::config_home() / "projctl/config.ini";
	data_path   = SystemInterface::data_home() / "projctl/projects.ini";
	load();
}
