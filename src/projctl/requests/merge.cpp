#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::request_merge(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const std::expected result = GitHostInterface::request_merge((*project)->second.path.string());
	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), symbol(Style::Mark::Cross), result.error(), reset())
		);
	}
	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}

auto ProjCtl::handle(const Cmd::GH::RequestMerge& cmd) const -> int {
	return report(request_merge(cmd.name));
}
