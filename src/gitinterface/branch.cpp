#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::branch(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" branch --show-current", project.path.string());
	std::optional<std::string> output = SystemInterface::run(command);

	if (!output) [[unlikely]] {
		return std::unexpected("Git remote failed");
	}
	return *output;
}


std::expected<std::string, std::string> GitInterface::branch_list(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" branch --list", project.path.string());
	std::optional<std::string> output = SystemInterface::run(command);

	if (!output) [[unlikely]] {
		return std::unexpected("Git remote failed");
	}
	return *output;
}


std::expected<std::string, std::string> GitInterface::branch_switch(const ProjectContent& project, std::string_view branch) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" branch --list \"{}\" 2>&1", project.path.string(), branch);
	std::optional<std::string> output = SystemInterface::run(command);
	if (!output) [[unlikely]] {
		return std::unexpected("Git branch --list failed");
	}

	bool branch_exists = !output->empty();
	command = std::format("git -C \"{}\" switch {}\"{}\" 2>&1", project.path.string(), branch_exists ? "" : "-c " , branch);
	output = SystemInterface::run(command);
	if (!output) [[unlikely]] {
		return std::unexpected(branch_exists ? "Git branch switch failed" : "Git branch creation failed");
	}
	return *output;
}
