#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::default_merge(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" merge", project.path.string());
	std::optional<std::string> result = SystemInterface::run(command);

	if (!result) [[unlikely]] {
		return std::unexpected("Git merge failed");
	}
	return *result;
}
