#include "projctl.hpp"

#include "command.hpp"
#include "projectcontent.hpp"
#include "projecttype.hpp"
#include "style.hpp"
#include "systeminterface.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>
#include <vector>

auto ProjCtl::run(const std::optional<std::string>& name) const -> std::expected<int, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	const std::optional command_option = content.run_command.or_else([&] {
		return std::optional<std::string>{run_command_for(detect_project_type(content.path))};
	});

	if (!command_option) {
		return std::unexpected(
			std::format(
				"{}{} There's no default run command for {}{}{} as type of project {}{}{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				paint(config.colors.type),
				project_type_to_string(detect_project_type(content.path)),
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

auto ProjCtl::handle(const Cmd::Base::Run& cmd) const -> int {
	return report(run(cmd.name));
}
