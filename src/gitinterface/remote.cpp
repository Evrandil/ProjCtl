#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::remote(const ProjectContent& project)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "remote", "get-url", "origin"}, project.path.string());

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


std::expected<std::string, std::string> GitInterface::remote_short(const ProjectContent& project)
{
	std::expected<std::string, std::string> remote_output = remote(project);
	if (!remote_output) [[unlikely]]
	{
		return std::unexpected(remote_output.error());
	}

	std::string remote = *remote_output;
	if (remote.ends_with(".git"))
	{
		remote.erase(remote.size() - 4);
	}

	std::size_t separator = remote.find_last_of('/');
	if (separator == std::string::npos) [[unlikely]]
	{
		return std::unexpected("Something went wrong while parsing remote's name");
	}

	const std::string repo = remote.substr(separator + 1);

	remote.erase(separator);
	separator = remote.find_last_of("/:");
	if (separator == std::string::npos) [[unlikely]]
	{
		return std::unexpected("Something went wrong while parsing remote's userspace");
	}

	const std::string owner = remote.substr(separator + 1);

	return std::format("{}/{}", owner, repo);
}
