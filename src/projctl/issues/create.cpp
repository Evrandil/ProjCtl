#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>


auto ProjCtl::handle(const Cmd::GH::Issue::Create& cmd) const -> ResultInt {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const ResultInt result = GitHostInterface::issue_create((*project)->second.path.string());
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{}Failed to create issue: {}{}\n",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				result.error(),
				reset()
			)
		);
	}

	return *result;
}
