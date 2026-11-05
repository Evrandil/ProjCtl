#include "projctl.hpp"

#include "systeminterface.hpp"


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
