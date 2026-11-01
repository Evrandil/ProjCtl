#include "gitinterface.hpp"


std::expected<HostOption, std::string> GitInterface::host(const ProjectContent& project) {
	if (!is_repo(project)) [[unlikely]] {
		return std::unexpected("Not a git repo");
	}

	std::expected<std::string, std::string> remote_output = remote(project);
	if (!remote_output) [[unlikely]] {
		return std::unexpected(remote_output.error());
	}

	std::string remote = *remote_output;
	std::size_t separator = remote.find_last_of('/'); // repo name
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing host's name");
	}
	remote.erase(separator);

	separator = remote.find_last_of("/:"); // repo userspace
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing host's name");
	}
	remote.erase(separator);

	separator = remote.find_last_of('.'); // .com / other domain
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing host's name");
	}
	remote.erase(separator);

	separator = remote.find_last_of("@."); // hostname
	if (separator == std::string::npos) [[unlikely]] {
		return std::unexpected("Something went wrong while parsing host's name");
	}

	std::string host = remote.substr(separator +  1);
	if (host == "github")	{	return HostOption::GitHub;	}
	if (host == "gitlab")	{	return HostOption::GitLab;	}
	return HostOption::Unknown;
}
