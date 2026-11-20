#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"

#include <expected>
#include <format>
#include <optional>


auto ProjCtl::handle(const Cmd::Git::Commit& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	const ResultString result = GitInterface::commit(content.path, cmd.tracked_only, cmd.message);

	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, result.error(), reset())
		);
	}
	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}
