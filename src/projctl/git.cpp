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


std::string ProjCtl::status(const ProjectEntry& project) const
{
	constexpr uint8_t CONTENT_WIDTH = 24;
	std::string output;

	auto parse_line = [&](const std::string_view header, const std::optional<bool> tick, const std::string_view text, const Style::Color text_color)
	{
		std::format_to(std::back_inserter(output), "{}{}{:<{}}{}{}{}{}\n", MARGIN, color_code(config.header_color), header, config.name_width,
					   tick ? *tick ? color_code(config.tick_color) : color_code(config.cross_color) : color_code(text_color), tick ? *tick ? Style::TICK : Style::CROSS : "",
					   truncate(text, config.max_width - CONTENT_WIDTH), Style::RESET);
	};

	const std::string& name			= project.first;
	const ProjectContent& content	= project.second;
	const bool exists            	= std::filesystem::exists(content.path);

	parse_line("Name:", std::nullopt, name, config.project_name_color);
	parse_line("Path:", std::nullopt, content.path.string(), config.project_path_color);
	parse_line("Exists:", exists, exists ? "Yes" : "No", exists ? config.tick_color : config.cross_color);

	if (!exists)
	{
		return output;
	}

	parse_line("Type:", std::nullopt, project_type_to_string(content.type), config.project_type_color);
	const std::expected<std::string, std::string> git = GitInterface::remote_short(content);
	parse_line("Git:", git.has_value(), git ? *git : git.error(), git ? config.tick_color : config.cross_color);

	if (!git)
	{
		return output;
	}
	const std::expected<std::string, std::string> branch  = GitInterface::branch(content);
	const std::expected<std::string, std::string> remote  = GitInterface::remote(content);
	const std::expected<std::string, std::string> changes = GitInterface::changes(content);
	parse_line("Branch:", std::nullopt, branch ? *branch : branch.error(), branch ? config.project_branch_color : config.cross_color);
	parse_line("Full remote:", std::nullopt, remote ? *remote : remote.error(), remote ? config.project_remote_color : config.cross_color);
	parse_line("Status:", std::nullopt, changes ? changes->empty() ? "No changes" : "Changes:" : changes.error(),
			changes ? changes->empty() ? config.tick_color : config.warning_color : config.error_color);
	if (changes && !changes->empty())
	{
		std::format_to(std::back_inserter(output), "\n{}{}{}\n", color_code(config.warning_color), *changes, Style::RESET);
	}

	return output;
}


std::expected<std::string, std::string> ProjCtl::status(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	return status(**project);
}

std::expected<std::string, std::string> ProjCtl::status_all() const
{
	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const ProjectEntry& project : projects)
	{
		tasks.push_back(std::async(std::launch::async, [this, &project] { return status(project); }));
	}

	std::string output;
	for (std::future<std::string>& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}\n\n", task.get());
	}

	return output;
}



/*
	if (name == "--all")
	{
		std::string output;
		std::vector<std::future<std::string>> tasks;
		tasks.reserve(projects.size());
		for (const decltype(projects)::value_type& project : projects)
		{
			tasks.push_back(std::async(std::launch::async, status_one, std::cref(project.first), std::cref(project.second)));
		}
		for (std::future<std::string>& task : tasks)
		{
			std::format_to(std::back_inserter(output), "{}\n", task.get());
		}
		return output;
	}
*/


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


// TODO that one also get rid of --all flag and make another func for all I think
// And make result expected so if commit's not successful it returns the error instead of just printing it
std::expected<std::string, std::string> ProjCtl::git_pull(const std::optional<std::string>& name) const
{
	auto pull_one = [&](const std::string& project_name, const ProjectContent& content)
	{
		std::string output;
		std::format_to(std::back_inserter(output), "{}Pulling: {}{}{}\n", color_code(config.header_color), color_code(config.project_name_color), project_name, Style::RESET);
		std::expected<std::string, std::string> result = GitInterface::pull(content);

		std::format_to(std::back_inserter(output), "{}{}{}{}", result ? color_code(config.tick_color) : color_code(config.error_color), result ? Style::TICK : Style::CROSS,
					   result ? *result : result.error(), Style::RESET);
		return output;
	};

	if (name == "--all")
	{
		std::string output;
		std::vector<std::future<std::string>> tasks;
		tasks.reserve(projects.size());
		for (const decltype(projects)::value_type& project : projects)
		{
			if (GitInterface::is_repo(project.second))
			{
				tasks.push_back(std::async(std::launch::async, pull_one, std::cref(project.first), std::cref(project.second)));
			}
		}
		for (std::future<std::string>& task : tasks)
		{
			std::format_to(std::back_inserter(output), "{}\n", task.get());
		}
		return output;
	}
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return std::format("{}", pull_one((*project)->first, (*project)->second));
}


// TODO that one also get rid of --all flag and make another func for all I think
// And make result expected so if commit's not successful it returns the error instead of just printing it
std::expected<std::string, std::string> ProjCtl::git_fetch(const std::optional<std::string>& name) const
{
	auto fetch_one = [&](const std::string& project_name, const ProjectContent& content)
	{
		std::string output;
		std::format_to(std::back_inserter(output), "{}Fetching: {}{}{}\n", color_code(config.header_color), color_code(config.project_name_color), project_name, Style::RESET);
		std::expected<std::string, std::string> result = GitInterface::fetch(content);

		std::format_to(std::back_inserter(output), "{}{}{}{}", result ? color_code(config.tick_color) : color_code(config.error_color), result ? Style::TICK : Style::CROSS,
					   result ? *result : result.error(), Style::RESET);
		return output;
	};


	if (name == "--all")
	{
		std::string output;
		std::vector<std::future<std::string>> tasks;
		tasks.reserve(projects.size());
		for (const decltype(projects)::value_type& project : projects)
		{
			if (GitInterface::is_repo(project.second))
			{
				tasks.push_back(std::async(std::launch::async, fetch_one, std::cref(project.first), std::cref(project.second)));
			}
		}
		for (std::future<std::string>& task : tasks)
		{
			std::format_to(std::back_inserter(output), "{}\n", task.get());
		}
		return output;
	}


	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return std::format("{}", fetch_one((*project)->first, (*project)->second));
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
