#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::fetch(const ProjectContent& project)
{
	if (!is_repo(project)) [[unlikely]]
	{
		return std::unexpected("Not a git repo");
	}

	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "fetch"}, project.path.string());

	if (!result) [[unlikely]]
	{
		return std::unexpected(std::format("Git not accessible: {}", result.error()));
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	return result->text;
}
