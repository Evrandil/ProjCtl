#include "projctl.hpp"

#include <format>
#include <string_view>
#include <future>
#include <vector>
#include <functional>
#include <iterator>
#include "config.hpp"
#include "systeminterface.hpp"

namespace
{
	constexpr int MARGIN_SIZE = 20;
}


std::expected<std::string, std::string> ProjCtl::list() const
{
	auto list_one = [&](const std::string& name, const ProjectContent& content)
	{
		return std::format("{}{:<{}}{}{:<{}}{}\n", color_code(config.project_name_color), truncate(name, config.name_width), config.name_width, color_code(config.project_path_color),
						   truncate(content.path.string(), config.path_width), config.path_width, Style::RESET);
	};
	if (projects.empty()) [[unlikely]]
	{
		return std::unexpected(std::format("{}There are no projects saved{}\n", color_code(config.error_color), Style::RESET));
	}

	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const decltype(projects)::value_type& project : projects)
	{
		tasks.push_back(std::async(std::launch::async, list_one, std::cref(project.first), std::cref(project.second)));
	}


	std::string output;

	std::format_to(std::back_inserter(output), "{}{:<{}}{}{}\n", color_code(config.header_color), "Name", config.name_width, "Path", Style::RESET);

	for (std::future<std::string>& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}", task.get());
	}

	return output;
}


std::expected<std::string, std::string> ProjCtl::path_show(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return (*project)->second.path.string();
}


std::expected<std::string, std::string> ProjCtl::add(const std::optional<std::string>& name, std::optional<std::filesystem::path> path)
{
	if (name && projects.contains(*name))
	{
		return std::unexpected(std::format("{}Project with name {}{}{} already exists!{}", color_code(config.error_color), color_code(config.project_name_color), truncate(*name, config.name_width),
										   color_code(config.error_color), Style::RESET));
	}
	if (!path)
	{
		path = std::filesystem::current_path();
	}
	std::pair<std::string, ProjectContent> new_project = {*name, ProjectContent{*path, detect_project_type(*path)}};
	projects.emplace(new_project);

	return std::format("{}{:<{}}{}{}{}\n{}{:<{}}{}{}{}", color_code(config.header_color), "Added project:", config.name_width, color_code(config.project_name_color),
					   truncate(new_project.first, config.max_width - MARGIN_SIZE), Style::RESET, color_code(config.header_color), "With path:", config.name_width,
					   color_code(config.project_path_color), truncate(new_project.second.path.string(), config.max_width - MARGIN_SIZE), Style::RESET);
}


std::expected<std::string, std::string> ProjCtl::add_current()
{
	std::filesystem::path path = std::filesystem::current_path();
	std::string name           = std::filesystem::current_path().filename().string();

	if (projects.contains(name))
	{
		return std::unexpected(std::format("{}Project with name {}{}{} already exists!{}", color_code(config.error_color), color_code(config.project_name_color), truncate(name, config.name_width),
										   color_code(config.error_color), Style::RESET));
	}

	std::pair<std::string, ProjectContent> new_project = {name, {path, detect_project_type(path)}};
	projects.emplace(new_project);

	return std::format("{}{:<{}}{}{}{}\n{}{:<{}}{}{}{}", color_code(config.header_color), "Added project:", config.name_width, color_code(config.project_name_color),
					   truncate(new_project.first, config.max_width - MARGIN_SIZE), Style::RESET, color_code(config.header_color), "With path:", config.name_width,
					   color_code(config.project_path_color), truncate(new_project.second.path.string(), config.max_width - MARGIN_SIZE), Style::RESET);
}


std::expected<std::string, std::string> ProjCtl::remove(const std::string& name)
{
	if (projects.erase(name) == 0)
	{
		return std::unexpected(std::format("{}Project with name {}{}{} doesn't exist!{}", color_code(config.error_color), color_code(config.project_name_color), truncate(name, config.name_width),
										   color_code(config.error_color), Style::RESET));
	}

	return std::format("{}{:<{}}{}{}{}", color_code(config.header_color), "Removed project:", config.name_width, color_code(config.project_name_color), name, Style::RESET);
}


std::expected<std::string, std::string> ProjCtl::open(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	const bool background = config.editor == "neovide" || config.editor == "emacs";

	const std::string shell_command = std::format("{} .{}", config.editor, background ? " &" : "");

	std::expected<int, std::string> result = SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", shell_command}, (*project)->second.path);

	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (*result != 0)
	{
		return std::unexpected(std::format("{} exited with code {}", config.editor, *result));
	}

	return std::format("{}{}{}{}{}", color_code(config.header_color), "Opened project: ", color_code(config.project_name_color), *name, Style::RESET);
}


std::expected<int, std::string> ProjCtl::build(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	const ProjectContent& content = (*project)->second;

	const std::optional<std::string_view> command_option = content.build_command ? content.build_command : build_command_for(content.type);

	if (!command_option)
	{
		return std::unexpected(std::format("{}There's no default build command for {}{}{} as type of project {}{}{}", color_code(config.error_color), color_code(config.project_type_color),
										   project_type_to_string(content.type), color_code(config.error_color), color_code(config.project_name_color), *name, Style::RESET));
	}

	return SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", std::string{*command_option}}, content.path);
}


std::expected<int, std::string> ProjCtl::run(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	const std::optional<std::string_view> command_option = content.run_command ? content.run_command : run_command_for(content.type);

	if (!command_option)
	{
		return std::unexpected(std::format("{}There's no default run command for {}{}{} as type of project {}{}{}", color_code(config.error_color), color_code(config.project_type_color),
										   project_type_to_string(content.type), color_code(config.error_color), color_code(config.project_name_color), *name, Style::RESET));
	}

	return SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", std::string{*command_option}}, content.path);
}
