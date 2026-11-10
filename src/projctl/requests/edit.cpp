#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::request_edit(const std::optional<std::string>& name, const int request_number) const
	-> std::expected<int, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const std::expected result = GitHostInterface::request_edit((*project)->second.path.string(), request_number);
	if (!result) {
		return std::unexpected(
			std::format(
				"{}{}Failed to edit request {}: {}{}\n",
				paint(config.colors.error),
				symbol(Style::Mark::Cross),
				request_number,
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
