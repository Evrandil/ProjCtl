#include "projctl.hpp"


std::expected<std::string, std::string> ProjCtl::remove(const std::string& name)
{
	if (projects.erase(name) == 0)
	{
		return std::unexpected(std::format("{}Project with name {}{}{} doesn't exist!{}", paint(config.error_color), paint(config.project_name_color), truncate(name, config.name_width),
										   paint(config.error_color), reset()));
	}

	return std::format("{}{:<{}}{}{}{}", paint(config.header_color), "Removed project:", config.name_width, paint(config.project_name_color), name, reset());
}


int ProjCtl::handle(const Cmd::Base::Remove& cmd)
{
	return report_save(remove(cmd.name));
}
