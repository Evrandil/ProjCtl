#include "projctl.hpp"

#include "command.hpp"
#include "gitinterface.hpp"

#include <expected>


auto ProjCtl::handle(const Cmd::Git::Diff& cmd) const -> ResultInt {
	const std::expected project = project_find(cmd.name);
	if (!project) {
		return std::unexpected(project.error());
	}

	return GitInterface::diff((*project)->second.path, cmd.path);
}
