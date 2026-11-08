#include "command.hpp"
#include "projctl.hpp"

#include "projectcontent.hpp"
#include "style.hpp"
#include "systeminterface.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

auto ProjCtl::run(const std::optional<std::string>& name) const -> std::expected<int, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	const std::optional<std::string_view> command_option =
		content.run_command ? content.run_command : run_command_for(content.type);

	if (!command_option) {
		return std::unexpected(
			std::format(
				"{}{} There's no default run command for {}{}{} as type of project {}{}{}",
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

auto ProjCtl::handle(const Cmd::Base::Run& cmd) const -> int {
	return report(run(cmd.name));
}
