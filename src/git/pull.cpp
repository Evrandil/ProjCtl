#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


std::expected<std::string, std::string> GitInterface::pull(const ProjectContent& project)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "pull"}, project.path.string());

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
