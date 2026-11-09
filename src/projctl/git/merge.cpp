#include "command.hpp"
#include "projctl.hpp"

#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>

auto ProjCtl::git_merge(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result = GitInterface::default_merge(content.path);
	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.colors.error), Style::CROSS, result.error(), reset())
		);
	}

	return std::format("{}{}{}{}", paint(config.colors.tick), Style::TICK, *result, reset());
}

auto ProjCtl::handle(const Cmd::Git::Merge& cmd) const -> int {
	return report(git_merge(cmd.name));
}
