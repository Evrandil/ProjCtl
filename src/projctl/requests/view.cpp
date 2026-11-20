#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::request_view(const std::optional<std::string>& name, const int request_number) const
	-> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	std::expected request = GitHostInterface::request_view((*project)->second.path, request_number);
	if (!request) {
		return std::unexpected(
			std::format(
				"{}{}{}{}",
				paint(config.colors.error),
				Style::symbol(Style::Mark::Cross),
				request.error(),
				reset()
			)
		);
	}
	return render_row(*request, config.tables.request_columns);
}

auto ProjCtl::handle(const Cmd::GH::Request::View& cmd) const -> int {
	return report(request_view(cmd.name, cmd.number));
}
