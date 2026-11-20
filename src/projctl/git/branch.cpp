#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>


auto ProjCtl::handle(const Cmd::Git::Branch& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	if (!cmd.branch_name) {
		const std::expected result = GitInterface::branch(content.path);
		if (!result) {
			return std::unexpected(std::format("{}{}{}", paint(config.colors.error), result.error(), reset()));
		}

		return std::format("{}{}{}", paint(config.colors.branch), *result, reset());
	}

	const std::expected result = GitInterface::branch_switch(content.path, *cmd.branch_name);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}Couldn't switch branches for {}{}{}: {}{}",
				paint(config.colors.error),
				paint(config.colors.name),
				(*project)->first,
				paint(config.colors.error),
				result.error(),
				reset()
			)
		);
	}
	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}
