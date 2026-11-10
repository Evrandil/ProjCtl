#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"

#include <expected>
#include <optional>
#include <string>

auto ProjCtl::issue_view(const std::optional<std::string>& name, int issue_number) const
	-> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	return GitHostInterface::issue_view((*project)->second.path, issue_number);
}

auto ProjCtl::handle(const Cmd::GH::IssueView& cmd) const -> int {
	return report(issue_view(cmd.name, cmd.number));
}
