#include "projctl.hpp"


std::expected<std::string, std::string> ProjCtl::path_show(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return (*project)->second.path.string();
}
