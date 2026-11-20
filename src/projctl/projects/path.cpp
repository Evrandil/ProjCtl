#include "projctl.hpp"

#include "command.hpp"

#include <expected>
#include <print>

auto ProjCtl::handle(const Cmd::Base::Path& cmd) const -> ResultInt {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		std::println(stderr, "\n\n{}\n\n", project.error());
		return 1;
	}

	std::println("{}", (*project)->second.path.string());
	return 0;
}
