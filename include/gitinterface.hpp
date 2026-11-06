#pragma once

#include "projectcontent.hpp"
#include "githostinterface.hpp"

#include <expected>
#include <string>


namespace GitInterface
{
	[[nodiscard]] std::expected<std::string, std::string> add(const ProjectContent&);
	[[nodiscard]] bool is_repo(const ProjectContent&);
	[[nodiscard]] std::expected<HostOption, std::string> host(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> changes(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> branch(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> branch_list(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> branch_switch(const ProjectContent&, std::string_view branch);
	[[nodiscard]] std::expected<std::string, std::string> remote(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> remote_short(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> commit(const ProjectContent&, std::optional<std::string_view>);
	[[nodiscard]] std::expected<std::string, std::string> push(const ProjectContent&, bool upstream = false);
	[[nodiscard]] std::expected<std::string, std::string> pull(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> fetch(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> default_merge(const ProjectContent&);
	[[nodiscard]] std::expected<std::string, std::string> log(const ProjectContent&, bool oneline = false);
	[[nodiscard]] std::expected<int, std::string> diff(const ProjectContent&, const std::optional<std::filesystem::path>&);
}; // namespace GitInterface
