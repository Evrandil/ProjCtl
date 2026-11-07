#include "projctl.hpp"

#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::request_view(const std::optional<std::string>& name, int request_number) const
{
	const std::expected project = project_find(name);
	if (!project)
	{
		return std::unexpected(project.error());
	}
	return GitHostInterface::request_view((*project)->second.path, request_number);
}


int ProjCtl::handle(const Cmd::GH::RequestView& cmd) const
{
	return report(request_view(cmd.name, cmd.number));
}
