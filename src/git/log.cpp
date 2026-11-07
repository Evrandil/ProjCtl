#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


[[nodiscard]] std::expected<std::string, std::string> GitInterface::log(const std::filesystem::path& path, bool oneline)
{
	std::vector<std::string>command {"git", "log"};
	if (oneline)
	{
		command.push_back("--oneline");
	}
	std::expected result = SystemInterface::run(command, path);
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
