#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>

auto ProjCtl::handle(const Cmd::GH::Request::Edit& cmd) const -> ResultInt {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const ResultInt result = GitHostInterface::request_edit((*project)->second.path.string(), cmd.number);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{}Failed to edit request {}: {}{}\n",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				cmd.number,
				result.error(),
				reset()
			)
		);
	}

	return *result;
}
