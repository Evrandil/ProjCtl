#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


namespace
{
	// Does this exact ref exist? e.g. "refs/heads/main" or "refs/remotes/origin/feature"
	bool ref_exists(const std::filesystem::path& path, const std::string& branch)
	{
		std::expected result = SystemInterface::run(std::vector<std::string>{"git", "show-ref", "--verify", "--quiet", branch}, path);
		return result && result->exit_code == 0;
	}
} // namespace


std::expected<std::string, std::string> GitInterface::branch(const std::filesystem::path& path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "branch", "--show-current"}, path);

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


std::expected<std::string, std::string> GitInterface::branch_list(const std::filesystem::path& path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "branch", "--list"}, path);

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


std::expected<std::string, std::string> GitInterface::branch_switch(const std::filesystem::path& path, std::string_view branch)
{
	const std::string name{branch};

	const bool exists = ref_exists(path, "refs/heads/" + name) || ref_exists(path, "refs/remotes/origin/" + name);

	std::vector<std::string> args{"git", "switch"};
	if (!exists)
	{
		args.emplace_back("-c");
	}
	args.push_back(name);

	auto result = SystemInterface::run(args, path);
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
