#include "projctl.hpp"


std::expected<std::string, std::string> ProjCtl::add(const std::optional<std::string>& name, const std::optional<std::filesystem::path>& path)
{
	std::error_code error;
	const std::filesystem::path full_path = std::filesystem::weakly_canonical(std::filesystem::absolute(path.value_or(std::filesystem::current_path())), error);
	if (error || !std::filesystem::is_directory(full_path))
	{
		return std::unexpected(std::format("{}{}Path {}{}{} doesn't exist!{}", paint(config.error_color), Style::CROSS, paint(config.project_path_color), truncate(path.value_or(".").string(), config.max_width - MARGIN_SIZE),
										   paint(config.error_color), reset()));
	}

	const std::string actual_name = name.value_or(full_path.filename().string());
	if (actual_name.empty())
	{
		return std::unexpected(std::format("{}{}Can't derive project name from {}{}{}", paint(config.error_color), Style::CROSS, paint(config.project_path_color), path.value_or(".").string(), reset()));
	}


	const auto [entry, inserted] = projects.try_emplace(actual_name, ProjectContent{full_path, detect_project_type(full_path)});

	if (!inserted)
	{
		return std::unexpected(std::format("{}{}Project with name {}{}{} already exists!{}", paint(config.error_color), Style::CROSS, paint(config.project_name_color), truncate(actual_name, config.name_width),
										   paint(config.error_color), reset()));
	}

	return std::format("{}{:<{}}{}{}{}\n{}{:<{}}{}{}{}", paint(config.header_color), "Added project:", config.name_width, paint(config.project_name_color),
					   truncate(entry->first, config.max_width - MARGIN_SIZE), reset(), paint(config.header_color), "With path:", config.name_width,
					   paint(config.project_path_color), truncate(entry->second.path.string(), config.max_width - MARGIN_SIZE), reset());
}


int ProjCtl::handle(const Cmd::Base::Add& cmd)
{
	return report_save(add(cmd.name, cmd.path));
}
