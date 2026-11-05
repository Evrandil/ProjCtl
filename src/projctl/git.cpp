#include "projctl.hpp"

#include <format>
#include <string_view>
#include <future>
#include <vector>
#include <functional>
#include <iterator>
#include "config.hpp"
#include "gitinterface.hpp"


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






std::expected<std::string, std::string> ProjCtl::git_commit(const std::optional<std::string>& name, std::optional<std::string_view> message) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result;
	if (message)
	{
		result = GitInterface::commit(content, *message);
	}
	else
	{
		result = GitInterface::commit(content);
	}

	if (!result)
	{
		return std::unexpected(result.error());
	}
	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}


std::expected<std::string, std::string> ProjCtl::git_push(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::push(content);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}

std::expected<std::string, std::string> ProjCtl::git_push_branch(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;


	// TODO just make gitinterface push_branch return the branch name instead of taking it as an argument or do some
	// optional funky stuff here too
	std::expected<std::string, std::string> result = GitInterface::branch(content);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	std::string branch = *result;

	result = GitInterface::push_branch(content, branch);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}





// TODO for both branch funcs make some header for output
std::expected<std::string, std::string> ProjCtl::git_branch_list(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::branch_list(content);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	return std::format("{}{}{}", color_code(config.project_branch_color), *result, Style::RESET);
}


std::expected<std::string, std::string> ProjCtl::git_branch(const std::optional<std::string>& name, const std::optional<std::string>& branch) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::branch_switch(content, *branch);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	return std::format("{}{}{}{}", color_code(config.tick_color), Style::TICK, *result, Style::RESET);
}


std::expected<std::string, std::string> ProjCtl::git_merge(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::default_merge(content);
	if (!result)
	{
		return std::unexpected(result.error());
	}

	return std::format("{}{}{}", color_code(config.tick_color), Style::TICK, *result);
}
