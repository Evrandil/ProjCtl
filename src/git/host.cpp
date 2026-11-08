#include "gitinterface.hpp"

#include "remote_url.hpp"

auto GitInterface::host(const std::filesystem::path& path) -> std::expected<HostOption, std::string>
{
	std::expected<std::string, std::string> remote_output = remote(path);
	if (!remote_output) [[unlikely]]
	{
		return std::unexpected(remote_output.error());
	}

	return host_from_remote(*remote_output);
}
