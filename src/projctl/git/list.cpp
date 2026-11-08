#include "projctl.hpp"

#include "gitinterface.hpp"
#include <future>
#include <ranges>

auto ProjCtl::list_gits(bool names_only) const -> std::expected<std::string, std::string>
{
	auto list_one_git = [&](const std::string& name, const ProjectContent& content) -> std::string
	{
		std::expected branch       = GitInterface::branch(content.path);
		std::expected remote_short = GitInterface::remote_short(content.path);
		return std::format("{}{:<{}}{}{:<{}}{}{:<{}}{}{:<{}}\n", paint(config.project_name_color), truncate(name, config.name_width), config.name_width, paint(config.project_path_color),
		                   truncate(content.path.string(), config.path_width), config.path_width, paint(config.project_branch_color), truncate(branch.value_or("-"), config.branch_width), config.branch_width,
		                   paint(config.project_short_remote_color), truncate(remote_short.value_or("-"), config.remote_width), config.remote_width);
	};

	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	auto is_git = [](const ProjectEntry& project) -> bool { return GitInterface::is_repo(project.second.path); };

	std::string output;

	if (names_only)
	{
		for (const ProjectEntry& project : projects | std::views::filter(is_git))
		{
			std::format_to(std::back_inserter(output), "{}\n", project.first);
		}
		return output;
	}

	for (const ProjectEntry& project : projects | std::views::filter(is_git))
	{
		tasks.push_back(std::async(std::launch::async, list_one_git, std::cref(project.first), std::cref(project.second)));
	}

	if (tasks.empty()) [[unlikely]]
	{
		return std::unexpected(std::format("{}There are no git projects saved{}\n", paint(config.error_color), reset()));
	}

	std::format_to(std::back_inserter(output), "{}{:<{}}{:<{}}{:<{}}{:<{}}{}\n", paint(config.header_color), "Name", config.name_width, "Path", config.path_width, "Branch", config.branch_width, "Repo",
	               config.remote_width, reset());

	for (std::future<std::string>& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}", task.get());
	}
	return output;
}
