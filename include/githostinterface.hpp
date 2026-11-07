#pragma once

#include "issue.hpp"
#include "request.hpp"
#include "githostoption.hpp"

#include <vector>
#include <expected>
#include <string>




enum class CliState : uint8_t
{
	NotInstalled,
	NotAuthenticated,
	Ready,
};


namespace GitHostInterface
{
	[[nodiscard]] CliState state(HostOption);
	[[nodiscard]] std::expected<std::string, std::string> repo_list(HostOption);
	[[nodiscard]] std::expected<std::vector<Issue>, std::string> issue_list(HostOption, std::string_view);
	[[nodiscard]] std::expected<int, std::string> issue_create(HostOption, std::string_view);
	[[nodiscard]] std::expected<std::vector<Request>, std::string> request_list(HostOption, std::string_view);
	[[nodiscard]] std::expected<int, std::string> request_create(HostOption, std::string_view);
	[[nodiscard]] std::expected<std::string, std::string> request_merge(HostOption, std::string_view);
	[[nodiscard]] std::expected<int, std::string> issue_edit(HostOption, std::string_view, int);
	[[nodiscard]] std::expected<int, std::string> request_edit(HostOption, std::string_view, int);
}; // namespace GitHostInterface
