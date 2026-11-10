#include "gitinterface.hpp"

#include "githostoption.hpp"
#include "remote_url.hpp"

#include <expected>
#include <filesystem>
#include <string>

auto GitInterface::host(const std::filesystem::path& path) -> std::expected<HostOption, std::string> {
	std::expected remote_output = remote(path);
	if (!remote_output) [[unlikely]] {
		return std::unexpected(remote_output.error());
	}

	return host_from_remote(*remote_output);
}
