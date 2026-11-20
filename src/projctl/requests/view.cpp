#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>


auto ProjCtl::handle(const Cmd::GH::Request::View& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	std::expected request = GitHostInterface::request_view((*project)->second.path, cmd.number);
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
