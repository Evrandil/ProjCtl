#include "projctl.hpp"

#include "systeminterface.hpp"
#include <print>


std::expected<int, std::string> ProjCtl::run(const std::optional<std::string>& name) const
{
	std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project.error(), Style::RESET));
	}
	const ProjectContent& content = (*project)->second;

	const std::optional<std::string_view> command_option = content.run_command ? content.run_command : run_command_for(content.type);

	if (!command_option)
	{
		return std::unexpected(std::format("{}There's no default run command for {}{}{} as type of project {}{}{}", color_code(config.error_color), color_code(config.project_type_color),
										   project_type_to_string(content.type), color_code(config.error_color), color_code(config.project_name_color), (*project)->first, Style::RESET));
	}

	return SystemInterface::run_interactive(std::vector<std::string>{"sh", "-c", std::string{*command_option}}, content.path);
}


int ProjCtl::handle(const Cmd::Base::Run& cmd) const
{
	std::expected<int, std::string> result = run(cmd.name);

	if (!result)
	{
		std::println("\n\n{}\n\n", result.error());
		return 1;
	}

	return *result;
}
