#include "projctl.hpp"


std::optional<std::string> ProjCtl::current_project_name() const
{
	std::filesystem::path cwd = std::filesystem::weakly_canonical(std::filesystem::current_path());
	for (const decltype(projects)::value_type& project : projects)
	{
		std::filesystem::path project_path = std::filesystem::weakly_canonical(project.second.path);

		if (cwd == project_path)
		{
			return project.first;
		}
	}
	return std::nullopt;
}
