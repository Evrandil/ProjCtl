#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::issue_close(const std::optional<std::string>& name, const int issue_number) const
	-> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const std::expected result = GitHostInterface::issue_close((*project)->second.path.string(), issue_number);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{}Failed to Close issue {}: {}{}\n",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				issue_number,
				result.error(),
				reset()
			)
		);
	}

	return *result;
}

auto ProjCtl::handle(const Cmd::GH::IssueClose& cmd) const -> int {
	return report(issue_close(cmd.name, cmd.number));
}
