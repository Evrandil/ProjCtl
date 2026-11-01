#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::push(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" push 2>&1", project.path.string());

	std::optional<std::string> result = SystemInterface::run(command);

	if (!result) [[unlikely]] {
		return std::unexpected("Git push failed");
	}

	return *result;
}


std::expected<std::string, std::string> GitInterface::push_branch(const ProjectContent& project, std::string_view branch) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" push -u origin {} 2>&1", project.path.string(), branch);

	std::optional<std::string> result = SystemInterface::run(command);

	if (!result) [[unlikely]] {
		return std::unexpected("Git push failed");
	}

	return *result;
}
