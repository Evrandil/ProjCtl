#include "gitinterface.hpp"

#include <cstddef>
#include <expected>
#include <filesystem>
#include <format>
#include <string>
#include <vector>

#include "systeminterface.hpp"

auto GitInterface::remote(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "remote", "get-url", "origin"}, path);

	if (!result) [[unlikely]] {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}

auto GitInterface::remote_short(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected<std::string, std::string> remote_output = remote(path);
	if (!remote_output) [[unlikely]] {
		return std::unexpected(remote_output.error());
	}

	std::string remote = *remote_output;
	if (remote.ends_with(".git")) {
		remote.erase(remote.size() - 4);
	}

	std::size_t separator = remote.find_last_of('/');
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing remote's name");
	}

	const std::string repo = remote.substr(separator + 1);

	remote.erase(separator);
	separator = remote.find_last_of("/:");
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing remote's userspace");
	}

	const std::string owner = remote.substr(separator + 1);

	return std::format("{}/{}", owner, repo);
}
