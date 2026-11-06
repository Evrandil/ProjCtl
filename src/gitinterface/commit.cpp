#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


std::expected<std::string, std::string> GitInterface::add(const ProjectContent& project)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "add", "."}, project.path.string());

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


std::expected<std::string, std::string> GitInterface::commit(const ProjectContent& project, std::optional<std::string_view> message)
{
	std::expected add_result = add(project);
	if (!add_result) [[unlikely]]
	{
		return std::unexpected(add_result.error());
	}

	if (!message)
	{
		std::expected<int, std::string> exit_code = SystemInterface::run_interactive(std::vector<std::string>{"git", "commit"}, project.path);
		if (!exit_code)
		{
			return std::unexpected(exit_code.error());
		}
		if (*exit_code != 0)
		{
			return std::unexpected("Commit aborted");
		}

		return "Committed";
	}

	std::expected<SystemInterface::Output, std::string> result = SystemInterface::run(std::vector<std::string>{"git", "commit", "-m", std::string{ *message }}, project.path);
	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}
	return result->text;
}
