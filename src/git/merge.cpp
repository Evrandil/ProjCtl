#include "gitinterface.hpp"

#include "systeminterface.hpp"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

auto GitInterface::default_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "merge"}, path);

	if (!result) [[unlikely]] {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}
