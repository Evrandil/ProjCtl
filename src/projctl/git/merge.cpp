#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"

#include <expected>
#include <format>


auto ProjCtl::handle(const Cmd::Git::Merge& cmd) const -> ResultString {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	const std::expected result = GitInterface::default_merge(content.path);
	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, result.error(), reset())
		);
	}

	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}
