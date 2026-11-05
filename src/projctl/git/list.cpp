#include "projctl.hpp"

#include "gitinterface.hpp"
#include <future>


std::expected<std::string, std::string> ProjCtl::list_gits() const
{
	auto list_one_git = [&](const std::string& name, const ProjectContent& content)
	{
		return std::format("{}{:<{}}{}{:<{}}{}{:<{}}{}{:<{}}\n", color_code(config.project_name_color), truncate(name, config.name_width), config.name_width, color_code(config.project_path_color),
						   truncate(content.path.string(), config.path_width), config.path_width, color_code(config.project_branch_color),
						   truncate(*GitInterface::branch(content), config.branch_width), config.branch_width, color_code(config.project_short_remote_color),
						   truncate(*GitInterface::remote_short(content), config.remote_width), config.remote_width);
	};

	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const decltype(projects)::value_type& project : projects)
	{
		if (GitInterface::is_repo(project.second))
		{
			tasks.push_back(std::async(std::launch::async, list_one_git, std::cref(project.first), std::cref(project.second)));
		}
	}

	if (tasks.empty()) [[unlikely]]
	{
		return std::unexpected(std::format("{}There are no git projects saved{}\n", color_code(config.error_color), Style::RESET));
	}

	std::string output;

	std::format_to(std::back_inserter(output), "{}{:<{}}{:<{}}{:<{}}{:<{}}{}\n", color_code(config.header_color), "Name", config.name_width, "Path", config.path_width, "Branch", config.branch_width,
				   "Repo", config.remote_width, Style::RESET);

	for (std::future<std::string>& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}", task.get());
	}
	return output;
}
