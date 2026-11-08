#include "projctl.hpp"

#include "gitinterface.hpp"

auto ProjCtl::diff(const std::optional<std::string>& name, const std::optional<std::filesystem::path>& path) const -> std::expected<int, std::string>
{
	auto project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}

	return GitInterface::diff((*project)->second.path, path);
}

auto ProjCtl::handle(const Cmd::Git::Diff& cmd) const -> int
{
	return report(diff(cmd.name, cmd.path));
}
