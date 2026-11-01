#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::add(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" add . 2>&1", project.path.string());
	std::optional<std::string> result = SystemInterface::run(command);

	if (!result) [[unlikely]] {
		return std::unexpected("Git add failed");
	}
	return *result;
}


std::expected<std::string, std::string> GitInterface::commit(const ProjectContent& project, std::string_view message) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Project is not git repository");
	}

	std::expected<std::string, std::string> add_output = add(project);
	if (!add_output) [[unlikely]] {
		return std::unexpected(add_output.error());
	}

	std::string command = std::format("git -C \"{}\" commit -m \"{}\" 2>&1", project.path.string(), message);

	std::optional<std::string> commit_output = SystemInterface::run(command);
	if (!commit_output) [[unlikely]] {
		return std::unexpected("Git commit failed");
	}

	add_output->append("\n");
	add_output->append(*commit_output);

	return *add_output;
}


std::expected<std::string, std::string> GitInterface::commit(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Project is not git repository");
	}

	std::expected<std::string, std::string> add_output = add(project);
	if (!add_output) [[unlikely]] {
		return std::unexpected(add_output.error());
	}

	std::string command = std::format("git -C \"{}\" commit 2>&1", project.path.string());

	int commit_output = SystemInterface::run_interactive(command);
	if (commit_output != 0) {
		return std::unexpected("Git commit failed");
	}

	add_output->append("\n");
	add_output->append("Git commit completed successfully");

	return *add_output;
}
