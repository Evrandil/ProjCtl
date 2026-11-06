#include "projctl.hpp"

#include <future>
#include <ranges>
#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::fetch(const ProjectEntry& project) const
{
	std::string output = std::format("{}Fetching: {}{}{}\n", color_code(config.header_color), color_code(config.project_name_color), project.first, Style::RESET);
	std::expected<std::string, std::string> result = GitInterface::fetch(project.second);
	if (!result)
	{
		std::format_to(std::back_inserter(output), "{}{}Couldn't fetch {}{}{}: {}{}", color_code(config.error_color), Style::CROSS, color_code(config.project_name_color), project.first, color_code(config.error_color), result.error(), Style::RESET);
		return std::unexpected(output);
	}
	std::format_to(std::back_inserter(output), "{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
	return output;
}

std::expected<std::string, std::string> ProjCtl::fetch(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
	}

	return fetch(**project);
}

BatchResult ProjCtl::fetch_all() const
{
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second); };

	for (const ProjectEntry& project : projects | std::views::filter(is_git))
	{
		tasks.push_back(std::async(std::launch::async, [this, &project] {return fetch(project); }));
	}

	BatchResult results;
	for (std::future<std::expected<std::string, std::string>>& task : tasks)
	{
		std::expected fetched = task.get();
		if (!fetched)
		{
			++results.failed;
			std::format_to(std::back_inserter(results.output), "{}\n\n", fetched.error());
			continue;
		}
		std::format_to(std::back_inserter(results.output), "{}\n\n", *fetched);
	}

	return results;
}


int ProjCtl::handle(const Cmd::Git::Fetch& cmd) const
{
	return cmd.all ? report(fetch_all()) : report(fetch(cmd.name));
}
