#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<int, std::string> ProjCtl::diff(const std::optional<std::string>& name, const std::optional<std::filesystem::path>& path) const
{
	auto project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return GitInterface::diff((*project)->second.path, path);
}


int ProjCtl::handle(const Cmd::Git::Diff& cmd) const
{
	return report(diff(cmd.name, cmd.path));
}
