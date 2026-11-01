#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::fetch(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" fetch 2>&1", project.path.string());

	std::optional<std::string> result = SystemInterface::run(command);

	if (!result) [[unlikely]] {
		return std::unexpected("Git fetch failed");
	}

	return *result;
}
