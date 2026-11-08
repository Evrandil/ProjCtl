#include "projctl.hpp"

auto ProjCtl::list(bool names_only) const -> std::expected<std::string, std::string>
{
	auto list_one = [&](const std::string& name, const std::filesystem::path& path) -> std::string
	{
		return std::format("{}{:<{}}{}{:<{}}{}\n", paint(config.project_name_color), truncate(name, config.name_width), config.name_width, paint(config.project_path_color), truncate(path.string(), config.path_width),
		                   config.path_width, reset());
	};

	std::string output;

	if (names_only)
	{
		for (const ProjectEntry& project : projects)
		{
			std::format_to(std::back_inserter(output), "{}\n", project.first);
		}
		return output;
	}

	if (projects.empty()) [[unlikely]]
	{
		return std::unexpected(std::format("{}{}There are no projects saved{}\n", paint(config.error_color), symbol(Style::Mark::Cross), reset()));
	}

	std::format_to(std::back_inserter(output), "{}{:<{}}{}{}\n", paint(config.header_color), "Name", config.name_width, "Path", reset());

	for (const ProjectEntry& project : projects)
	{
		std::format_to(std::back_inserter(output), "{}", list_one(project.first, project.second.path));
	}

	return output;
}

auto ProjCtl::handle(const Cmd::Base::List& cmd) const -> int
{
	return cmd.gits_only ? report(list_gits(cmd.names_only)) : report(list(cmd.names_only));
}
