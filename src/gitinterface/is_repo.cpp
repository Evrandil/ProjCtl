#include "gitinterface.hpp"

#include "systeminterface.hpp"


bool GitInterface::is_repo(const ProjectContent& project)
{
	std::expected result= SystemInterface::run(std::vector<std::string>{"git", "rev-parse", "--is-inside-work-tree"}, project.path.string());
	return result && result->text == "true";
}
