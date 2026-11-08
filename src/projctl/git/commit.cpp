#include "command.hpp"
#include "projctl.hpp"

#include "gitinterface.hpp"
#include "projectcontent.hpp"
#include "style.hpp"
#include <expected>
#include <format>
#include <optional>
#include <string>
#include <string_view>

auto ProjCtl::commit(const std::optional<std::string>& name, std::optional<std::string_view> message) const
	-> std::expected<std::string, std::string> {
	std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}
	const ProjectContent& content = (*project)->second;

	std::expected<std::string, std::string> result;
	result = GitInterface::commit(content.path, message);

	if (!result) {
		return std::unexpected(
			std::format("{}{}{}{}", paint(config.error_color), Style::CROSS, result.error(), reset())
		);
	}
	return std::format("{}{}{}{}", paint(config.tick_color), Style::TICK, *result, reset());
}

auto ProjCtl::handle(const Cmd::Git::Commit& cmd) const -> int {
	return report(commit(cmd.name, cmd.message));
}
