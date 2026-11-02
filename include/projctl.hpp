#pragma once

#include "projectcontent.hpp"
#include "config.hpp"
#include "command.hpp"
#include <filesystem>
#include <map>
#include <expected>

constexpr std::string_view MARGIN = "    ";
constexpr int MARGIN_WIDTH = 20;

ProjectType detect_project_type(const std::filesystem::path&);
std::string trim(std::string_view);
std::string truncate(std::string_view, size_t);

class ProjCtl {
	std::map<std::string, ProjectContent> projects;
	std::filesystem::path config_path;
	std::filesystem::path data_path;
	Config config;
	void load();
	void save() const;
	std::expected<std::map<std::string, ProjectContent>::const_iterator, std::string> project_find(const std::string&) const;
	std::optional<std::string> current_project_name() const;

	// Base functionality
	std::expected<std::string,	std::string> list() const;
	std::expected<std::string,	std::string> list_gits() const;
	std::expected<std::string,	std::string> path_show(const std::string&) const;
	std::expected<std::string,	std::string> add(const std::string&, std::filesystem::path);
	std::expected<std::string,	std::string> add_current();
	std::expected<std::string,	std::string> remove(const std::string&);
	std::expected<std::string,	std::string> open(const std::string&) const;
	std::expected<int,			std::string> build(const std::string&) const;
	std::expected<int,			std::string> run(const std::string&) const;

	// Git functionality
	std::expected<std::string, std::string> status(const std::string&) const;
	std::expected<std::string, std::string> git_commit(const std::string&, std::optional<std::string_view>) const;
	std::expected<std::string, std::string> git_push(const std::string&) const;
	std::expected<std::string, std::string> git_push_branch(const std::string&) const;
	std::expected<std::string, std::string> git_pull(const std::string&) const;
	std::expected<std::string, std::string> git_fetch(const std::string&) const;
	std::expected<std::string, std::string> git_branch_list(const std::string&) const;
	std::expected<std::string, std::string> git_branch(const std::string&, std::string_view) const;
	std::expected<std::string, std::string> git_merge(const std::string&) const;

	// GH/GLab functionality
//	void g_status() const;
//	void repo_list() const;
//	void issue_list(const std::string&) const;
//	void issue_create(const std::string&) const;
//	void request_list(const std::string&) const;
//	void request_create(const std::string&) const;
//	void request_merge(const std::string&) const;

	// Base functionality
	int handle(const Cmd::Unknown&) const;
	int handle(const Cmd::Base::List&) const;
	int handle(const Cmd::Base::Path&) const;
	int handle(const Cmd::Base::Add&);
	int handle(const Cmd::Base::Remove&);
	int handle(const Cmd::Base::Open&) const;
	int handle(const Cmd::Base::Build&) const;
	int handle(const Cmd::Base::Run&) const;
	int handle(const Cmd::Git::Status&) const;
	int handle(const Cmd::Git::Commit&) const;
	int handle(const Cmd::Git::Push&) const;
	int handle(const Cmd::Git::Pull&) const;
	int handle(const Cmd::Git::Fetch&) const;
	int handle(const Cmd::Git::Branch&) const;
	int handle(const Cmd::Git::Merge&) const;

	public:
	ProjCtl();
	~ProjCtl();
	int execute(const Command& command) {
		return std::visit([this] (const auto& cmd) { return handle(cmd); }, command);
	}

};
