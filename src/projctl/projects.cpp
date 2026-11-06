#include "projctl.hpp"

#include "config.hpp"




std::expected<std::string, std::string> ProjCtl::remove(const std::string& name)
{
	if (projects.erase(name) == 0)
	{
		return std::unexpected(std::format("{}Project with name {}{}{} doesn't exist!{}", color_code(config.error_color), color_code(config.project_name_color), truncate(name, config.name_width),
										   color_code(config.error_color), Style::RESET));
	}

	return std::format("{}{:<{}}{}{}{}", color_code(config.header_color), "Removed project:", config.name_width, color_code(config.project_name_color), name, Style::RESET);
}
