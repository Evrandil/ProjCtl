#include "projctl.hpp"

#include "command.hpp"
#include "projectcontent.hpp"
#include "style.hpp"
#include "systeminterface.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

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
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.type),
				project_type_to_string(content.type),
				paint(config.colors.error),
				paint(config.colors.name),
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
