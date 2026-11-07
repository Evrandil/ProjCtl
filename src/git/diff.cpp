#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


std::expected<int, std::string> GitInterface::diff(const std::filesystem::path& path, const std::optional<std::filesystem::path>& file_path)
{
	std::vector<std::string> args{"git", "diff"};

	if (file_path)
	{
		args.push_back(file_path->string());
	}

	return SystemInterface::run_interactive(args, path);
}
