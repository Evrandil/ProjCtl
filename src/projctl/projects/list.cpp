#include "projctl.hpp"

#include <future>


std::expected<std::string, std::string> ProjCtl::list() const
{
	auto list_one = [&](const std::string& name, const ProjectContent& content)
	{
		return std::format("{}{:<{}}{}{:<{}}{}\n", paint(config.project_name_color), truncate(name, config.name_width), config.name_width, paint(config.project_path_color),
						   truncate(content.path.string(), config.path_width), config.path_width, reset());
	};
	if (projects.empty()) [[unlikely]]
	{
		return std::unexpected(std::format("{}There are no projects saved{}\n", paint(config.error_color), reset()));
	}

	std::vector<std::future<std::string>> tasks;
	tasks.reserve(projects.size());

	for (const decltype(projects)::value_type& project : projects)
	{
		tasks.push_back(std::async(std::launch::async, list_one, std::cref(project.first), std::cref(project.second)));
	}


	std::string output;

	std::format_to(std::back_inserter(output), "{}{:<{}}{}{}\n", paint(config.header_color), "Name", config.name_width, "Path", reset());

	for (std::future<std::string>& task : tasks)
	{
		std::format_to(std::back_inserter(output), "{}", task.get());
	}

	return output;
}


int ProjCtl::handle(const Cmd::Base::List& cmd) const
{
	if (cmd.gits_only)
	{
		return report(list_gits());
	}
	return report(list());
}
