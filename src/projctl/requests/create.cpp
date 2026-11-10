#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::request_create(const std::optional<std::string>& name) const -> std::expected<int, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	std::expected result = GitHostInterface::request_create((*project)->second.path.string());
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{}Failed to create request: {}{}",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				result.error(),
				reset()
			)
		);
	}

	return *result;
}

auto ProjCtl::handle(const Cmd::GH::RequestCreate& cmd) const -> int {
	return report(request_create(cmd.name));
}
