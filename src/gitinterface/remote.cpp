#include "gitinterface.hpp"

#include "systeminterface.hpp"


std::expected<std::string, std::string> GitInterface::remote(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::string command = std::format("git -C \"{}\" remote get-url origin", project.path.string());
	std::optional<std::string> output = SystemInterface::run(command);

	if (!output) [[unlikely]] {
		return std::unexpected("Git remote failed");
	}
	return *output;
}


std::expected<std::string, std::string> GitInterface::remote_short(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::expected<std::string, std::string> remote_output = remote(project);
	if (!remote_output) [[unlikely]] {
		return std::unexpected(remote_output.error());
	}

	std::string remote = *remote_output;
	if (remote.ends_with(".git")) {
		remote.erase(remote.size() - 4);
	}

	std::size_t separator = remote.find_last_of('/');
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing remote's name");
	}

	const std::string repo = remote.substr(separator + 1);

	remote.erase(separator);
	separator = remote.find_last_of("/:");
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing remote's userspace");
	}

	const std::string owner = remote.substr(separator + 1);

	return std::format("{}/{}", owner, repo);
}
