#include "projctl.hpp"

#include "gitinterface.hpp"
#include <print>


std::expected<int, std::string> ProjCtl::request_create(const std::optional<std::string>& name) const
{
	std::expected project_option = project_find(name);
	if (!project_option)
	{
		return std::unexpected(std::format("{}{}{}{}", color_code(config.error_color), Style::CROSS, project_option.error(), Style::RESET));
	}

	const ProjectContent& content                      = (*project_option)->second;
	std::expected<HostOption, std::string> host_option = GitInterface::host(content);
	if (!host_option)
	{
		return std::unexpected(std::format("{}Failed to get host {}{}", color_code(config.error_color), host_option.error(), Style::RESET));
	}
	HostOption host = *host_option;

	return GitHostInterface::request_create(host, content.path.string());
}


int ProjCtl::handle(const Cmd::GH::RequestCreate& cmd) const
{
	std::expected<int, std::string> result = request_create(cmd.name);

	if (!result)
	{
		std::println("\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	return *result;
}
