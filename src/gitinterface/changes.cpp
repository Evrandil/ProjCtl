#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::changes(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" status --porcelain", project.path.string());
	std::optional<std::string> output = SystemInterface::run(command);

	if (!output) [[unlikely]] {
		return std::unexpected("Git status failed");
	}
	return *output;
}
