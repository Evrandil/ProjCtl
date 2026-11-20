#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>

auto ProjCtl::handle(const Cmd::Git::Push& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const ResultString result = GitInterface::push((*project)->second.path, cmd.branch_upstream);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}Couldn't push {}{}{}: {}{}",
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
