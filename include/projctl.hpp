#pragma once

#include "command.hpp"
#include "config.hpp"
#include "projectcontent.hpp"
#include <expected>
#include <filesystem>
#include <map>
#include <vector>

constexpr std::string_view MARGIN       = "    ";
constexpr int              MARGIN_WIDTH = 20;
constexpr int              MARGIN_SIZE  = 20;

auto detect_project_type(const std::filesystem::path&) -> ProjectType;
auto trim(std::string_view) -> std::string;
auto truncate(std::string_view, size_t) -> std::string;
using ProjectEntry    = std::map<std::string, ProjectContent>::value_type;
using ProjectIterator = std::map<std::string, ProjectContent>::const_iterator;

struct BatchResult
{
	std::string output;
	std::size_t failed = 0;
};

class ProjCtl
{
	std::map<std::string, ProjectContent> projects;
	std::filesystem::path                 config_path;
	std::filesystem::path                 data_path;
	Config                                config;

	bool use_colors = true;
	auto paint(Style::Color color) const -> std::string_view;
	auto reset() const -> std::string_view;

	void               load();
	[[nodiscard]] auto save() const noexcept -> std::expected<void, std::string>;
	[[nodiscard]] auto project_find(const std::optional<std::string>&) const -> std::expected<ProjectIterator, std::string>;
	[[nodiscard]] auto current_project_name() const -> std::optional<std::string>;

	[[nodiscard]] auto report(const std::expected<std::string, std::string>&) const -> int;
	[[nodiscard]] auto report(const std::expected<int, std::string>&) const -> int;
	[[nodiscard]] auto report(const BatchResult&) const -> int;
	[[nodiscard]] auto report_save(const std::expected<std::string, std::string>&) const -> int;

	// Program's functionality
	[[nodiscard]] auto handle(const Cmd::Unknown&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Version&) const -> int;

	// Base functionality
	[[nodiscard]] auto list(bool) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto list_gits(bool) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto path_show(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto add(const std::optional<std::string>&, const std::optional<std::filesystem::path>&) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto remove(const std::string&) -> std::expected<std::string, std::string>;
	[[nodiscard]] auto open(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto build(const std::optional<std::string>&) const -> std::expected<int, std::string>;
	[[nodiscard]] auto run(const std::optional<std::string>&) const -> std::expected<int, std::string>;
	[[nodiscard]] auto handle(const Cmd::Base::List&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Path&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Add&) -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Remove&) -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Open&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Build&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Base::Run&) const -> int;

	// Git functionality
	[[nodiscard]] auto status(const ProjectEntry&) const -> std::string;
	[[nodiscard]] auto status(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto status_all() const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto fetch(const ProjectEntry&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto fetch(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto fetch_all() const -> BatchResult;
	[[nodiscard]] auto pull(const ProjectEntry&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto pull(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto pull_all() const -> BatchResult;
	[[nodiscard]] auto git_commit(const std::optional<std::string>&, std::optional<std::string_view>) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto push(const std::optional<std::string>&, bool) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto branch(const std::optional<std::string>&, const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto git_merge(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto log(const std::optional<std::string>&, bool) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto diff(const std::optional<std::string>&, const std::optional<std::filesystem::path>&) const -> std::expected<int, std::string>;
	[[nodiscard]] auto handle(const Cmd::Git::Status&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Fetch&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Commit&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Push&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Pull&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Branch&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Merge&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Log&) const -> int;
	[[nodiscard]] auto handle(const Cmd::Git::Diff&) const -> int;

	// GH functionality
	[[nodiscard]] auto g_status() const -> BatchResult;
	[[nodiscard]] auto repo_list() const -> BatchResult;
	[[nodiscard]] auto issue_table(const std::vector<Issue>&) const -> std::string;
	[[nodiscard]] auto issues(const ProjectEntry&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto issues(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto issues_all() const -> BatchResult;
	[[nodiscard]] auto issue_create(const std::optional<std::string>&) const -> std::expected<int, std::string>;
	[[nodiscard]] auto issue_edit(const std::optional<std::string>&, int) const -> std::expected<int, std::string>;
	[[nodiscard]] auto issue_view(const std::optional<std::string>&, int) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto issue_close(const std::optional<std::string>&, int) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_table(const std::vector<Request>&) const -> std::string;
	[[nodiscard]] auto requests(const ProjectEntry&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto requests(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto requests_all() const -> BatchResult;
	[[nodiscard]] auto request_create(const std::optional<std::string>&) const -> std::expected<int, std::string>;
	[[nodiscard]] auto request_edit(const std::optional<std::string>&, int) const -> std::expected<int, std::string>;
	[[nodiscard]] auto request_view(const std::optional<std::string>&, int) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_close(const std::optional<std::string>&, int) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto request_merge(const std::optional<std::string>&) const -> std::expected<std::string, std::string>;
	[[nodiscard]] auto handle(const Cmd::GH::Status&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RepoList&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueList&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueCreate&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueEdit&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueView&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::IssueClose&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestList&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestCreate&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestEdit&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestView&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestClose&) const -> int;
	[[nodiscard]] auto handle(const Cmd::GH::RequestMerge&) const -> int;

	public:
	ProjCtl();

	auto execute(const Command& command) -> int
	{
		return std::visit([this](const auto& cmd) -> auto { return handle(cmd); }, command);
	}
};
