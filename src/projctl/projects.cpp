#include "projctl.hpp"

#include "config.hpp"


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
