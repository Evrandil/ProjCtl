#include "remote_url.hpp"

#include <format>

namespace {
	struct RemoteParts {
		std::string_view host;
		std::string_view path;
	};

	// Splits a remote URL into host and path. Two shapes:
	//   scheme://[user@]host[:port]/path     (https, http, ssh, git)
	//   [user@]host:path                     (scp-like ssh)
	auto split_remote(std::string_view remote) -> std::expected<RemoteParts, std::string> {
		if (const std::size_t scheme_end = remote.find("://"); scheme_end != std::string_view::npos) {
			const std::string_view rest = remote.substr(scheme_end + 3);

			const std::size_t path_start = rest.find('/');
			if (path_start == std::string_view::npos) {
				return std::unexpected(std::format("No repository path in {}", remote));
			}

			std::string_view authority = rest.substr(0, path_start); // user@host:port
			if (const std::size_t at_sign = authority.find('@'); at_sign != std::string_view::npos) {
				authority.remove_prefix(at_sign + 1); // host:port
			}
			const std::string_view host = authority.substr(0, authority.find(':')); // host

			if (host.empty()) {
				return std::unexpected(std::format("No host in {}", remote));
			}
			return RemoteParts{.host = host, .path = rest.substr(path_start + 1)};
		}

		std::string_view rest = remote;
		if (const std::size_t at_sign = rest.find('@'); at_sign != std::string_view::npos) {
			rest.remove_prefix(at_sign + 1); // host:path
		}

		const std::size_t colon = rest.find(':');
		if (colon == std::string_view::npos || colon == 0) {
			return std::unexpected(std::format("Not a remote URL: {}", remote));
		}
		return RemoteParts{.host = rest.substr(0, colon), .path = rest.substr(colon + 1)};
	}
} // namespace

auto host_from_remote(std::string_view remote) -> std::expected<HostOption, std::string> {
	const std::expected<RemoteParts, std::string> parts = split_remote(remote);
	if (!parts) {
		return std::unexpected(parts.error());
	}

	if (parts->host.contains("github")) {
		return HostOption::GitHub;
	}
	if (parts->host.contains("gitlab")) {
		return HostOption::GitLab;
	}
	return HostOption::Unknown;
}

auto short_from_remote(std::string_view remote) -> std::expected<std::string_view, std::string> {
	const std::expected<RemoteParts, std::string> parts = split_remote(remote);
	if (!parts) {
		return std::unexpected(parts.error());
	}

	std::string_view path = parts->path;
	while (path.ends_with('/')) {
		path.remove_suffix(1);
	}
	if (path.ends_with(".git")) {
		path.remove_suffix(4);
	}

	if (!path.contains('/')) {
		return std::unexpected(std::format("No owner/repo in {}", remote));
	}
	return path;
}
