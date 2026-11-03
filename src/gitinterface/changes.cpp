#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::changes(const ProjectContent& project)
{
	if (!is_repo(project)) [[unlikely]]
	{
		return std::unexpected("Not a git repo");
	}

	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "status", "--porcelain"}, project.path.string());

	if (!result) [[unlikely]]
	{
		return std::unexpected(std::format("Git not available: {}", result.error()));
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	return result->text;
}
