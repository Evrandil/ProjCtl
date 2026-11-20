#include "projctl.hpp"

#include "command.hpp"
#include "githostinterface.hpp"
#include "style.hpp"

#include <expected>
#include <format>


auto ProjCtl::handle(const Cmd::GH::Request::Merge& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	const ResultString result = GitHostInterface::request_merge((*project)->second.path.string());
	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), symbol(Style::Mark::Cross), result.error(), reset())
		);
	}
	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}
