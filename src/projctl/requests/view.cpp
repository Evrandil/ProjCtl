#include "projctl.hpp"

#include "githostinterface.hpp"

auto ProjCtl::request_view(const std::optional<std::string>& name, int request_number) const -> std::expected<std::string, std::string>
{
	const std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	return GitHostInterface::request_view((*project)->second.path, request_number);
}

auto ProjCtl::handle(const Cmd::GH::RequestView& cmd) const -> int
{
	return report(request_view(cmd.name, cmd.number));
}
