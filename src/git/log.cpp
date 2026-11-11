#include "gitinterface.hpp"

#include "systeminterface.hpp"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

auto GitInterface::log(const std::filesystem::path& path, const bool oneline)
	-> std::expected<std::string, std::string> {
	std::vector<std::string> command{"git", "log"};
	if (oneline) {
		command.emplace_back("--oneline");
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
