#include "gitinterface.hpp"

#include "systeminterface.hpp"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

auto GitInterface::push(const std::filesystem::path& path, const bool upstream) -> std::expected<std::string, std::string> {
	std::vector<std::string> command = {"git", "push"};
	if (upstream) {
		command.insert(command.end(), {"-u", "origin", "HEAD"});
	}

	std::expected result = SystemInterface::run(command, path);

	if (!result) [[unlikely]] {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}
