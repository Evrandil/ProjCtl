#include "projctl.hpp"

#include "command.hpp"

#include <expected>
#include <optional>
#include <print>
#include <string>

auto ProjCtl::path_show(const std::optional<std::string>& name) const -> std::expected<std::string, std::string> {
	const std::expected project = project_find(name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return (*project)->second.path.string();
}

auto ProjCtl::handle(const Cmd::Base::Path& cmd) const -> int {
	const std::expected result = path_show(cmd.name);

	if (!result) {
		std::println(stderr, "\n\n{}\n\n", result.error());
		return 1;
	}

	std::println("{}", *result);
	return 0;
}
