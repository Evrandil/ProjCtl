#pragma once

#include "issue.hpp"
#include "request.hpp"

#include <vector>
#include <expected>
#include <string>


enum class HostOption : uint8_t {
	GitHub,
	GitLab,
	Unknown,
};


namespace GitHostInterface {
	[[nodiscard]] bool installed(HostOption);
	[[nodiscard]] bool authenticated(HostOption);
	[[nodiscard]] std::expected<std::string, std::string>repo_list(HostOption);
	[[nodiscard]] std::expected<std::vector<Issue>, std::string>issue_list(HostOption, std::string_view);
	[[nodiscard]] std::expected<void, std::string>issue_create(HostOption, std::string_view);
	[[nodiscard]] std::expected<std::vector<Request>, std::string> request_list(HostOption, std::string_view);
	[[nodiscard]] std::expected<void, std::string> request_create(HostOption, std::string_view);
	[[nodiscard]] std::expected<std::string, std::string> request_merge(HostOption, std::string_view);
};
