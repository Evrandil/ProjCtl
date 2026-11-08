#include "command.hpp"
#include "projctl.hpp"

#include "githostinterface.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::request_close(const std::optional<std::string>& name, int request_number) const
	-> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	std::expected result = GitHostInterface::request_close((*project)->second.path.string(), request_number);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{} Failed to close request {}: {}{}",
				paint(config.error_color),
				symbol(Style::Mark::Cross),
				request_number,
				result.error(),
				reset()
			)
		);
	}
	return *result;
}

auto ProjCtl::handle(const Cmd::GH::RequestClose& cmd) const -> int {
	return report(request_close(cmd.name, cmd.number));
}
