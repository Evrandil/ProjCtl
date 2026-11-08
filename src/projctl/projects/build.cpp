#include "projctl.hpp"

#include "systeminterface.hpp"

auto ProjCtl::build(const std::optional<std::string>& name) const -> std::expected<int, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const ProjectContent& content = (*project)->second;

	const std::optional<std::string_view> command_option =
		content.build_command ? content.build_command : build_command_for(content.type);

	if (!command_option) {
		return std::unexpected(
			std::format(
				"{}{}There's no default build command for {}{}{} as type of project {}{}{}",
				paint(config.error_color),
				symbol(Style::Mark::Cross),
				paint(config.project_type_color),
				project_type_to_string(content.type),
				paint(config.error_color),
				paint(config.project_name_color),
				(*project)->first,
				reset()
			)
		);
	}

	return SystemInterface::run_interactive(
		std::vector<std::string>{"sh", "-c", std::string{*command_option}},
		content.path
	);
}

auto ProjCtl::handle(const Cmd::Base::Build& cmd) const -> int {
	return report(build(cmd.name));
}
