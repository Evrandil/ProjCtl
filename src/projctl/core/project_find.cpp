#include "projctl.hpp"



std::expected<ProjectIterator, std::string> ProjCtl::project_find(const std::optional<std::string>& name) const
{
	std::string actual_name;
	if (name)
	{
		actual_name = *name;
	}
	else
	{
		std::optional<std::string> current = current_project_name();
		if (!current)
		{
			return std::unexpected(std::format("{}{}Current directory isn't a saved project{}", color_code(config.error_color), Style::CROSS, Style::RESET));
		}
		actual_name = *current;
	}
	std::map<std::string, ProjectContent>::const_iterator project = projects.find(actual_name);
	if (project == projects.end())
	{
		return std::unexpected(std::format("{}{}Project with name {}{}{} doesn't exist{}", color_code(config.error_color), Style::CROSS, color_code(config.project_name_color), actual_name, color_code(config.error_color), Style::RESET));
	}
	return project;
}
