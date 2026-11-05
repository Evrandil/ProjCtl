#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::push(const ProjectContent& project, bool upstream)
{
	std::vector<std::string> command = {"git", "push"};
	if (upstream)
	{
		command.insert(command.end(), {"-u", "origin", "HEAD"});
	}

	std::expected result = SystemInterface::run(command, project.path.string());

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
