#include "projctl.hpp"

#include <future>


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
