#include "command.hpp"
#include "projctl.hpp"

#include "gitinterface.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::log(const std::optional<std::string>& name, bool oneline) const
	-> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	std::expected result = GitInterface::log((*project)->second.path, oneline);
	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, result.error(), reset())
		);
	}
	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}

auto ProjCtl::handle(const Cmd::Git::Log& cmd) const -> int {
	return report(log(cmd.name, cmd.oneline));
}
