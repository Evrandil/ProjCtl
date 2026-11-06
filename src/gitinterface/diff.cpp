#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<int, std::string> GitInterface::diff(const ProjectContent& project, const std::optional<std::filesystem::path>& path)
{
	std::vector<std::string> args{"git", "diff"};

	if (path)
	{
		args.push_back(path->string());
	}

	return SystemInterface::run_interactive(args, project.path.string());
}
