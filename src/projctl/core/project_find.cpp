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
			return std::unexpected("Current directory isn't a saved project");
		}
		actual_name = *current;
	}
	std::map<std::string, ProjectContent>::const_iterator project = projects.find(actual_name);
	if (project == projects.end())
	{
		return std::unexpected(std::format("Project with name {} doesn't exist", actual_name));
	}
	return project;
}
