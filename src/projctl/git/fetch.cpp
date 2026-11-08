#include "projctl.hpp"

#include "gitinterface.hpp"
#include <future>
#include <ranges>

auto ProjCtl::fetch(const ProjectEntry& project) const -> std::expected<std::string, std::string>
{
	std::string                             output = std::format("{}Fetching: {}{}{}\n", paint(config.header_color), paint(config.project_name_color), project.first, reset());
	std::expected<std::string, std::string> result = GitInterface::fetch(project.second.path);
	if (!result)
	{
		std::format_to(
			std::back_inserter(output), "{}{}Couldn't fetch {}{}{}: {}{}", paint(config.error_color), Style::CROSS, paint(config.project_name_color), project.first, paint(config.error_color), result.error(), reset()
		);
		return std::unexpected(output);
	}
	std::format_to(std::back_inserter(output), "{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
	return output;
}

auto ProjCtl::fetch(const std::optional<std::string>& name) const -> std::expected<std::string, std::string>
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return fetch(**project);
}

auto ProjCtl::fetch_all() const -> BatchResult
{
	std::vector<std::future<std::expected<std::string, std::string>>> tasks;
	tasks.reserve(projects.size());

	auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	for (const ProjectEntry& project : projects | std::views::filter(is_git))
	{
		tasks.push_back(std::async(std::launch::async, [this, &project] -> std::expected<std::string, std::string> { return fetch(project); }));
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

auto ProjCtl::handle(const Cmd::Git::Fetch& cmd) const -> int
{
	return cmd.all ? report(fetch_all()) : report(fetch(cmd.name));
}
