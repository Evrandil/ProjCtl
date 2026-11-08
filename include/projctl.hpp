#pragma once

#include <expected>
#include <filesystem>
#include <map>
#include <vector>
#include <string>
#include <optional>
#include <variant>
#include <string_view>
#include <cstdlib>

#include "style.hpp"
#include "issue.hpp"
#include "request.hpp"
#include "command.hpp"
#include "config.hpp"
#include "projectcontent.hpp"

constexpr std::string_view MARGIN       = "    ";
constexpr int              MARGIN_WIDTH = 20;
constexpr int              MARGIN_SIZE  = 20;

auto detect_project_type(const std::filesystem::path& path) -> ProjectType;
auto trim(std::string_view text) -> std::string;
auto truncate(std::string_view text, size_t max_length) -> std::string;

using ProjectEntry    = std::map<std::string, ProjectContent>::value_type;
using ProjectIterator = std::map<std::string, ProjectContent>::const_iterator;

using ResultString = std::expected<std::string, std::string>;
using ResultInt    = std::expected<int, std::string>;

struct BatchResult {
	std::string output;
	std::size_t failed = 0;
};

class ProjCtl {
	std::map<std::string, ProjectContent> projects;
	std::filesystem::path                 config_path;
	std::filesystem::path                 data_path;
	Config                                config;

	bool               use_colors = true;
	[[nodiscard]] auto paint(Style::Color color) const -> std::string_view;
	[[nodiscard]] auto reset() const -> std::string_view;

	auto               load() -> void;
	[[nodiscard]] auto save() const -> std::expected<void, std::string>;
	[[nodiscard]] auto project_find(const std::optional<std::string>& name) const
		-> std::expected<ProjectIterator, std::string>;
	[[nodiscard]] auto current_project_name() const -> std::optional<std::string>;

	[[nodiscard]] static auto report(const ResultString& result) -> int;
	[[nodiscard]] static auto report(const ResultInt& result) -> int;
	[[nodiscard]] auto        report(const BatchResult& result) const -> int;
	[[nodiscard]] auto        report_save(const ResultString& result) const -> int;

	// Program's functionality
	[[nodiscard]] auto handle(const Cmd::Unknown& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Version& /*unused*/) const -> int;

	// Base functionality
	[[nodiscard]] auto list(bool names_only) const -> ResultString;
	[[nodiscard]] auto list_gits(bool names_only) const -> ResultString;
	[[nodiscard]] auto path_show(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto add(const std::optional<std::string>& name, const std::optional<std::filesystem::path>& path)
		-> ResultString;
	[[nodiscard]] auto remove(const std::string& name) -> ResultString;
	[[nodiscard]] auto open(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto build(const std::optional<std::string>& name) const -> ResultInt;
	[[nodiscard]] auto run(const std::optional<std::string>& name) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::Base::List& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Path& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Add& cmd) -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Remove& cmd) -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Open& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Build& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Run& cmd) const -> int;
	// clang-format on

	// Git functionality
	[[nodiscard]] auto status(const ProjectEntry& project) const -> std::string;
	[[nodiscard]] auto fetch(const ProjectEntry& project) const -> ResultString;
	[[nodiscard]] auto pull(const ProjectEntry& project) const -> ResultString;
	[[nodiscard]] auto status_all() const -> ResultString;
	[[nodiscard]] auto fetch_all() const -> BatchResult;
	[[nodiscard]] auto pull_all() const -> BatchResult;
	[[nodiscard]] auto status(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto fetch(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto pull(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto push(const std::optional<std::string>& name, bool upstream) const -> ResultString;
	[[nodiscard]] auto git_merge(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto log(const std::optional<std::string>& name, bool oneline) const -> ResultString;
	[[nodiscard]] auto
	branch(const std::optional<std::string>& name, const std::optional<std::string>& branch_name) const -> ResultString;
	[[nodiscard]] auto commit(const std::optional<std::string>& name, std::optional<std::string_view> message) const
		-> ResultString;
	[[nodiscard]] auto
	diff(const std::optional<std::string>& name, const std::optional<std::filesystem::path>& path) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::Git::Status& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Fetch& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Commit& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Push& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Pull& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Branch& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Merge& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Log& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Diff& cmd) const -> int;

	// GH functionality
	[[nodiscard]] auto g_status() const -> BatchResult;
	[[nodiscard]] auto repo_list() const -> BatchResult;
	[[nodiscard]] auto issue_table(const std::vector<Issue>& issues) const -> std::string;
	[[nodiscard]] auto request_table(const std::vector<Request>& requests) const -> std::string;
	[[nodiscard]] auto issues(const ProjectEntry& project) const -> ResultString;
	[[nodiscard]] auto requests(const ProjectEntry& project) const -> ResultString;
	[[nodiscard]] auto issues(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto requests(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto issues_all() const -> BatchResult;
	[[nodiscard]] auto requests_all() const -> BatchResult;
	[[nodiscard]] auto issue_create(const std::optional<std::string>& name) const -> ResultInt;
	[[nodiscard]] auto request_create(const std::optional<std::string>& name) const -> ResultInt;
	[[nodiscard]] auto issue_edit(const std::optional<std::string>& name, int issue_number) const -> ResultInt;
	[[nodiscard]] auto request_edit(const std::optional<std::string>& name, int request_number) const -> ResultInt;
	[[nodiscard]] auto issue_view(const std::optional<std::string>& name, int issue_number) const -> ResultString;
	[[nodiscard]] auto request_view(const std::optional<std::string>& name, int request_number) const -> ResultString;
	[[nodiscard]] auto issue_close(const std::optional<std::string>& name, int issue_number) const -> ResultString;
	[[nodiscard]] auto request_close(const std::optional<std::string>& name, int request_number) const -> ResultString;
	[[nodiscard]] auto request_merge(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Status& /*unused*/) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RepoList& /*unused*/) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueList& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueCreate& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueEdit& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueView& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueClose& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestList& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestCreate& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestEdit& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestView& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestClose& cmd) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestMerge& cmd) const -> int;

	public:
	ProjCtl();

	auto execute(const Command& command) -> int {
		return std::visit([this](const auto& cmd) -> auto { return handle(cmd); }, command);
	}
};
