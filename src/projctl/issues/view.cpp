#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"

#include <expected>
#include <optional>
#include <string>

auto ProjCtl::issue_view(const std::optional<std::string>& name, const int issue_number) const -> ResultString {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const std::expected issue = GitHostInterface::issue_view((*project)->second.path, issue_number);
	if (!issue) {
		return std::unexpected(issue.error());
	}
	return render_row(*issue, config.tables.issue_columns);
}

auto ProjCtl::handle(const Cmd::GH::IssueView& cmd) const -> int {
	return report(issue_view(cmd.name, cmd.number));
}
