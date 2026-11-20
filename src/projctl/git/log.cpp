#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>

auto ProjCtl::handle(const Cmd::Git::Log& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const ResultString result = GitInterface::log((*project)->second.path, cmd.oneline);
	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, result.error(), reset())
		);
	}
	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}
