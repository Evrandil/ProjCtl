#include "projctl.hpp"

#include <future>
#include <ranges>
#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::pull(const ProjectEntry& project) const
{
	std::string output;
	std::format_to(std::back_inserter(output), "{}Pulling: {}{}{}\n", paint(config.header_color), paint(config.project_name_color), project.first, reset());
	std::expected<std::string, std::string> result = GitInterface::pull(project.second.path);

	if (!result)
	{
		std::format_to(std::back_inserter(output), "{}{}Couldn't pull {}{}{}: {}{}", paint(config.error_color), Style::CROSS, paint(config.project_name_color), project.first, paint(config.error_color), result.error(), reset());
		return std::unexpected(output);
	}

	std::format_to(std::back_inserter(output), "{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
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

	auto is_git = [](const ProjectEntry& project) { return GitInterface::is_repo(project.second.path); };

	for (const ProjectEntry& project : projects | std::views::filter(is_git))
	{
		tasks.push_back(std::async(std::launch::async, [this, &project] {return pull(project); }));
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


int ProjCtl::handle(const Cmd::Git::Pull& cmd) const
{
	return cmd.all ? report(pull_all()) : report(pull(cmd.name));
}
