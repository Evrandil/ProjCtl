#include "projctl.hpp"

#include "gitinterface.hpp"
#include "githostinterface.hpp"


std::expected<std::string, std::string> ProjCtl::request_close(const std::optional<std::string>& name, int request_number) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(project_option.error());
	}

	const ProjectContent& content                      = (*project_option)->second;
	std::expected host_option = GitInterface::host(content.path);
	if (!host_option)
	{
		return std::unexpected(std::format("{}Failed to get host {}{}\n", paint(config.error_color), host_option.error(), reset()));
	}
	HostOption host = *host_option;

	return GitHostInterface::request_close(host, content.path.string(), request_number);
}


int ProjCtl::handle(const Cmd::GH::RequestClose& cmd) const
{
	return report(request_close(cmd.name, cmd.number));
}
