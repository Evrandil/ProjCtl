#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


std::expected<std::string, std::string> GitInterface::pull(const std::filesystem::path& path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "pull"}, path);

	if (!result) [[unlikely]]
	{
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	return result->text;
}
