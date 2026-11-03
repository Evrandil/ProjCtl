#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::default_merge(const ProjectContent& project)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "merge"}, project.path.string());

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
