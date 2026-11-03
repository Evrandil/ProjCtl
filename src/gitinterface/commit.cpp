#include "gitinterface.hpp"

#include "systeminterface.hpp"


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


std::expected<std::string, std::string> GitInterface::commit(const ProjectContent& project, std::string_view message)
{
	std::expected add_result = add(project);
	if (!add_result) [[unlikely]]
	{
		return std::unexpected(add_result.error());
	}

	std::expected commit_result = SystemInterface::run(std::vector<std::string>{"git", "-C", "commit", "-m", std::string(message)}, project.path.string());
	if (!commit_result) [[unlikely]]
	{
		return std::unexpected(commit_result.error());
	}
	if (commit_result->exit_code != 0)
	{
		return std::unexpected(commit_result->text);
	}

	add_result->append("\n");
	add_result->append(commit_result->text);

	return *add_result;
}


std::expected<std::string, std::string> GitInterface::commit(const ProjectContent& project)
{
	std::expected add_result = add(project);
	if (!add_result) [[unlikely]]
	{
		return std::unexpected(add_result.error());
	}

	std::expected commit_result = SystemInterface::run_interactive(std::vector<std::string>{"git", "commit"}, project.path.string());
	if (!commit_result) [[unlikely]]
	{
		return std::unexpected(commit_result.error());
	}
	if (*commit_result != 0)
	{
		return std::unexpected("Git commit failed");
	}

	return *add_result;
}
