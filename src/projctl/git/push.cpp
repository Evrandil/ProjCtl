#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::push(const std::optional<std::string>& name, const bool upstream) const
	-> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	std::expected result = GitInterface::push((*project)->second.path, upstream);
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

auto ProjCtl::handle(const Cmd::Git::Push& cmd) const -> int {
	return report(push(cmd.name, cmd.branch_upstream));
}
