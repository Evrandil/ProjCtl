#include "projctl.hpp"

#include "gitinterface.hpp"


std::expected<std::string, std::string> ProjCtl::request_merge(const std::optional<std::string>& name) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(project_option.error());
	}

	const ProjectContent& content                      = (*project_option)->second;
	std::expected<HostOption, std::string> host_option = GitInterface::host(content);
	if (!host_option)
	{
		return std::unexpected(std::format("{}Failed to get host {}{}", color_code(config.error_color), host_option.error(), Style::RESET));
	}
	HostOption host = *host_option;

	return GitHostInterface::request_merge(host, content.path.string());
}


int ProjCtl::handle(const Cmd::GH::RequestMerge& cmd) const
{
	return report(request_merge(cmd.name));
}
