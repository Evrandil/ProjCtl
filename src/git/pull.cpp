#include "gitinterface.hpp"

#include <vector>

#include "systeminterface.hpp"

auto GitInterface::pull(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "pull"}, path);

	if (!result) [[unlikely]] {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}
