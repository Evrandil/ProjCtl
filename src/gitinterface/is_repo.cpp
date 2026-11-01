#include "gitinterface.hpp"

#include "systeminterface.hpp"


bool GitInterface::is_repo(const ProjectContent& project) {
	std::string command = std::format("git -C \"{}\" rev-parse --is-inside-work-tree", project.path.string());
	std::optional<std::string> output = SystemInterface::run(command);
	return output && *output == "true";
}
