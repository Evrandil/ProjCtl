#pragma once

#include "command.hpp"
#include "config.hpp"
#include "projectcontent.hpp"
#include "row.hpp"
#include "style.hpp"

#include <cstddef>
#include <expected>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

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
	const std::filesystem::path           config_path;
	const std::filesystem::path           data_path;

	const Config       config;
	const bool         use_colors;
	[[nodiscard]] auto paint(Style::Color color) const -> std::string_view;
	[[nodiscard]] auto reset() const -> std::string_view;

	auto               load_projects() -> void;
	[[nodiscard]] auto save() const -> std::expected<void, std::string>;
	[[nodiscard]] auto project_find(const std::optional<std::string>& name) const
		-> std::expected<ProjectIterator, std::string>;
	[[nodiscard]] auto current_project_name() const -> std::optional<std::string>;

	[[nodiscard]] auto        batch_to_string(const BatchResult& results) const -> ResultString;
	[[nodiscard]] auto        with_save(const ResultString& result) const -> ResultString;
	[[nodiscard]] static auto report(const ResultString& result) -> int;
	[[nodiscard]] static auto report(const ResultInt& result) -> int;

	// Program's functionality
	[[nodiscard]] auto handle(const Cmd::Unknown& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::Version& /*unused*/) const -> ResultString;

	// Base functionality
	[[nodiscard]] auto list(bool names_only) const -> ResultString;
	[[nodiscard]] auto list_gits(bool names_only) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Base::List& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Base::Path& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::Base::Add& cmd) -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Base::Remove& cmd) -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Base::Open& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Base::Build& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::Base::Run& cmd) const -> ResultInt;

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
	[[nodiscard]] auto handle(const Cmd::Git::Status& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Fetch& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Commit& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Push& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Pull& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Branch& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Merge& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Log& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::Git::Diff& cmd) const -> ResultInt;

	// GH functionality
	[[nodiscard]] auto render_table(const std::vector<Row>& rows, const std::vector<TableField>& columns) const
		-> std::string;
	[[nodiscard]] auto render_row(const Row& row, const std::vector<TableField>& fields) const -> std::string;
	[[nodiscard]] auto issues(const ProjectEntry& project) const -> ResultString;
	[[nodiscard]] auto requests(const ProjectEntry& project) const -> ResultString;
	[[nodiscard]] auto issues(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto requests(const std::optional<std::string>& name) const -> ResultString;
	[[nodiscard]] auto issues_all() const -> BatchResult;
	[[nodiscard]] auto requests_all() const -> BatchResult;
	[[nodiscard]] auto handle(const Cmd::GH::Status& /*unused*/) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::RepoList& /*unused*/) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Issue::List& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Issue::Create& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::GH::Issue::Edit& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::GH::Issue::View& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Issue::Close& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Request::List& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Request::Create& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::GH::Request::Edit& cmd) const -> ResultInt;
	[[nodiscard]] auto handle(const Cmd::GH::Request::View& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Request::Close& cmd) const -> ResultString;
	[[nodiscard]] auto handle(const Cmd::GH::Request::Merge& cmd) const -> ResultString;

	public:
	ProjCtl();
	~ProjCtl()                                 = default;
	ProjCtl(const ProjCtl&)                    = delete;
	auto operator=(const ProjCtl&) -> ProjCtl& = delete;
	ProjCtl(ProjCtl&&)                         = delete;
	auto operator=(ProjCtl&&) -> ProjCtl&&     = delete;

	auto execute(const Command& command) -> int {
		return std::visit([this](const auto& cmd) -> int { return report(handle(cmd)); }, command);
	}
};
