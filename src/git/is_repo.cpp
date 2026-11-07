#include "gitinterface.hpp"

#include "systeminterface.hpp"
#include <vector>


bool GitInterface::is_repo(const std::filesystem::path& path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"git", "rev-parse", "--is-inside-work-tree"}, path);
	return result && result->text == "true";
}
