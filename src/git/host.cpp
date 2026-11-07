#include "gitinterface.hpp"

#include "remote_url.hpp"


std::expected<HostOption, std::string> GitInterface::host(const ProjectContent& project)
{
	std::expected<std::string, std::string> remote_output = remote(project);
	if (!remote_output) [[unlikely]]
	{
		return std::unexpected(remote_output.error());
	}

	return host_from_remote(*remote_output);
}
