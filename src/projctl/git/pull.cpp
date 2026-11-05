#include "projctl.hpp"

#include <future>
#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::pull(const ProjectEntry& project) const
{
	std::string output;
	std::format_to(std::back_inserter(output), "{}Pulling: {}{}{}\n", color_code(config.header_color), color_code(config.project_name_color), project.first, Style::RESET);
	std::expected<std::string, std::string> result = GitInterface::pull(project.second);

	if (!result)
	{
		std::format_to(std::back_inserter(output), "{}{}Couldn't pull {}{}{}: {}{}", color_code(config.error_color), Style::CROSS, color_code(config.project_name_color), project.first, color_code(config.error_color), result.error(), Style::RESET);
		return std::unexpected(output);
	}

	std::format_to(std::back_inserter(output), "{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
	return output;
}


std::expected<std::string, std::string> ProjCtl::pull(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return pull(**project);
}


BatchResult ProjCtl::pull_all() const
{
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	for (const ProjectEntry& project : projects)
	{
		if (GitInterface::is_repo(project.second))
		{
			tasks.push_back(std::async(std::launch::async, [this, &project] {return pull(project); }));
		}
	}

	BatchResult results;
	for (std::future<std::expected<std::string, std::string>>& task : tasks)
	{
		std::expected pulled = task.get();
		if (!pulled)
		{
			std::format_to(std::back_inserter(results.output), "{}\n", pulled.error());
			++results.failed;
			continue;
		}
		std::format_to(std::back_inserter(results.output), "{}\n\n", *pulled);
	}

	return results;
}
