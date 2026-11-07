#pragma once

#include "projectcontent.hpp"
#include "config.hpp"
#include "command.hpp"
#include <filesystem>
#include <map>
#include <expected>
#include <vector>


constexpr std::string_view MARGIN	= "    ";
constexpr int MARGIN_WIDTH			= 20;
constexpr int MARGIN_SIZE			= 20;


ProjectType detect_project_type(const std::filesystem::path&);
std::string trim(std::string_view);
std::string truncate(std::string_view, size_t);
using ProjectEntry = std::map<std::string, ProjectContent>::value_type;
using ProjectIterator = std::map<std::string, ProjectContent>::const_iterator;

struct BatchResult
{
	std::string output;
	std::size_t failed = 0;
};



class ProjCtl
{
	std::map<std::string, ProjectContent> projects;
	std::filesystem::path config_path;
	std::filesystem::path data_path;
	Config config;

	bool use_colors = true;
	std::string_view paint(Style::Color color) const;
	std::string_view reset() const;

	void load();
	[[nodiscard]] std::expected<void, std::string> save() const noexcept;
	[[nodiscard]] std::expected<ProjectIterator, std::string> project_find(const std::optional<std::string>&) const;
	[[nodiscard]] std::optional<std::string> current_project_name() const;

	[[nodiscard]] int report(const std::expected<std::string, std::string>&) const;
	[[nodiscard]] int report(const std::expected<int, std::string>&) const;
	[[nodiscard]] int report(const BatchResult&) const;
	[[nodiscard]] int report_save(const std::expected<std::string, std::string>&) const;

	// Bug functionality
	[[nodiscard]] int handle(const Cmd::Unknown&) const;

	// Base functionality
	[[nodiscard]] std::expected<std::string, std::string> list() const;
	[[nodiscard]] std::expected<std::string, std::string> list_gits() const;
	[[nodiscard]] std::expected<std::string, std::string> path_show(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<std::string, std::string> add(const std::optional<std::string>&, const std::optional<std::filesystem::path>&);
	[[nodiscard]] std::expected<std::string, std::string> remove(const std::string&);
	[[nodiscard]] std::expected<std::string, std::string> open(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<int, std::string> build(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<int, std::string> run(const std::optional<std::string>&) const;
	[[nodiscard]] int handle(const Cmd::Base::List&) const;
	[[nodiscard]] int handle(const Cmd::Base::Path&) const;
	[[nodiscard]] int handle(const Cmd::Base::Add&);
	[[nodiscard]] int handle(const Cmd::Base::Remove&);
	[[nodiscard]] int handle(const Cmd::Base::Open&) const;
	[[nodiscard]] int handle(const Cmd::Base::Build&) const;
	[[nodiscard]] int handle(const Cmd::Base::Run&) const;

	// Git functionality
	[[nodiscard]] std::string status(const ProjectEntry&) const;
	[[nodiscard]] std::expected<std::string, std::string> status(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<std::string, std::string> status_all() const;
	[[nodiscard]] std::expected<std::string, std::string> fetch(const ProjectEntry&) const;
	[[nodiscard]] std::expected<std::string, std::string> fetch(const std::optional<std::string>&) const;
	[[nodiscard]] BatchResult fetch_all() const;
	[[nodiscard]] std::expected<std::string, std::string> pull(const ProjectEntry&) const;
	[[nodiscard]] std::expected<std::string, std::string> pull(const std::optional<std::string>&) const;
	[[nodiscard]] BatchResult pull_all() const;
	[[nodiscard]] std::expected<std::string, std::string> git_commit(const std::optional<std::string>&, std::optional<std::string_view>) const;
	[[nodiscard]] std::expected<std::string, std::string> push(const std::optional<std::string>&, bool) const;
	[[nodiscard]] std::expected<std::string, std::string> branch(const std::optional<std::string>&, const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<std::string, std::string> git_merge(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<std::string, std::string> log(const std::optional<std::string>&, bool) const;
	[[nodiscard]] std::expected<int, std::string> diff(const std::optional<std::string>&, const std::optional<std::filesystem::path>&) const;
	[[nodiscard]] int handle(const Cmd::Git::Status&) const;
	[[nodiscard]] int handle(const Cmd::Git::Fetch&) const;
	[[nodiscard]] int handle(const Cmd::Git::Commit&) const;
	[[nodiscard]] int handle(const Cmd::Git::Push&) const;
	[[nodiscard]] int handle(const Cmd::Git::Pull&) const;
	[[nodiscard]] int handle(const Cmd::Git::Branch&) const;
	[[nodiscard]] int handle(const Cmd::Git::Merge&) const;
	[[nodiscard]] int handle(const Cmd::Git::Log&) const;
	[[nodiscard]] int handle(const Cmd::Git::Diff&) const;

	// GH functionality
	[[nodiscard]] BatchResult g_status() const;
	[[nodiscard]] BatchResult repo_list() const;
	[[nodiscard]] std::string issue_table(const std::vector<Issue>&) const;
	[[nodiscard]] std::expected<std::string, std::string> issues(const ProjectEntry&) const;
	[[nodiscard]] std::expected<std::string, std::string> issues(const std::optional<std::string>&) const;
	[[nodiscard]] BatchResult issues_all() const;
	[[nodiscard]] std::expected<int, std::string> issue_create(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<int, std::string> issue_edit(const std::optional<std::string>&, int) const;
	[[nodiscard]] std::string request_table(const std::vector<Request>&) const;
	[[nodiscard]] std::expected<std::string, std::string> requests(const ProjectEntry&) const;
	[[nodiscard]] std::expected<std::string, std::string> requests(const std::optional<std::string>&) const;
	[[nodiscard]] BatchResult requests_all() const;
	[[nodiscard]] std::expected<int, std::string> request_create(const std::optional<std::string>&) const;
	[[nodiscard]] std::expected<int, std::string> request_edit(const std::optional<std::string>&, int) const;
	[[nodiscard]] std::expected<std::string, std::string> request_merge(const std::optional<std::string>&) const;
	[[nodiscard]] int handle(const Cmd::GH::Status&) const;
	[[nodiscard]] int handle(const Cmd::GH::RepoList&) const;
	[[nodiscard]] int handle(const Cmd::GH::IssueList&) const;
	[[nodiscard]] int handle(const Cmd::GH::IssueCreate&) const;
	[[nodiscard]] int handle(const Cmd::GH::IssueEdit&) const;
	[[nodiscard]] int handle(const Cmd::GH::RequestList&) const;
	[[nodiscard]] int handle(const Cmd::GH::RequestCreate&) const;
	[[nodiscard]] int handle(const Cmd::GH::RequestEdit&) const;
	[[nodiscard]] int handle(const Cmd::GH::RequestMerge&) const;

	public:
	ProjCtl();
	int execute(const Command& command)
	{
		return std::visit([this](const auto& cmd) { return handle(cmd); }, command);
	}
};
