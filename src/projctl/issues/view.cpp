#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>


auto ProjCtl::handle(const Cmd::GH::Issue::View& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const std::expected issue = GitHostInterface::issue_view((*project)->second.path, cmd.number);
	if (!issue) {
		return std::unexpected(
			std::format(
				"{}{}{}{}",
				paint(config.colors.error),
				Style::symbol(Style::Mark::Cross),
				issue.error(),
				reset()
			)
		);
	}
	return render_row(*issue, config.tables.issue_columns);
}
