#include "command.hpp"
#include "projctl.hpp"

#include "githostinterface.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::request_edit(const std::optional<std::string>& name, int issue_number) const
	-> std::expected<int, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	std::expected result = GitHostInterface::request_edit((*project)->second.path.string(), issue_number);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{}Failed to edit request {}: {}{}\n",
				paint(config.error_color),
				symbol(Style::Mark::Cross),
				issue_number,
				result.error(),
				reset()
			)
		);
	}

	return *result;
}

auto ProjCtl::handle(const Cmd::GH::RequestEdit& cmd) const -> int {
	return report(request_edit(cmd.name, cmd.number));
}
