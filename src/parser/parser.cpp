#include "parser.hpp"
#include "command.hpp"

#include <charconv>
#include <expected>
#include <format>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <array>
#include <algorithm>

// grammar: see tests/parser_test.cpp

namespace {
	auto optional_name(std::span<const std::string_view> args)
		-> std::expected<std::optional<std::string>, std::string> {
		if (args.empty() || (args.size() == 1 && args.front() == ".")) {
			return std::nullopt;
		}
		if (args.size() > 1) {
			return std::unexpected(
				std::format("Too many arguments: expected at most one project name, got {}", args.size())
			);
		}
		if (args.front().starts_with('-')) {
			return std::unexpected(std::format("Unknown option: {}", args.front()));
		}
		if (args.front().contains('/')) {
			return std::unexpected(std::format("Invalid project name: {}", args.front()));
		}
		return std::string{args.front()};
	}

	template <typename CommandType>
	auto parser_with_name(std::span<const std::string_view> args) -> Command {
		const std::expected name = optional_name(args);
		if (!name) {
			return Cmd::Unknown{.message = name.error()};
		}
		return CommandType{.name = *name};
	}

	template <typename CommandType>
	auto with_name_or_all(std::span<const std::string_view> args) -> Command {
		if (args.size() == 1 && (args.front() == "--all" || args.front() == "-a")) {
			return CommandType{.all = true, .name = std::nullopt};
		}
		return parser_with_name<CommandType>(args);
	}

	auto parse_number(std::string_view text) -> std::optional<int> {
		int value               = 0;
		const char* const first = text.data();
		const char* const last  = std::to_address(text.end());

		const auto [end, error] = std::from_chars(first, last, value);

		if (error != std::errc{} || end != last || value <= 0) {
			return std::nullopt;
		}
		return value;
	}

	template <typename CommandType>
	auto number_with_optional_name(std::span<const std::string_view> args) -> Command {
		std::expected<std::optional<std::string>, std::string> name = std::nullopt;
		switch (args.size()) {
			case 2:
				name = optional_name(args.last(1));
				if (!name) {
					return Cmd::Unknown{.message = name.error()};
				}
				[[fallthrough]];
			case 1: {
				std::optional<int> number = parse_number(args.front());
				if (!number) {
					return Cmd::Unknown{.message = std::format("Invalid number: {}", args.front())};
				}

				return CommandType{
					.number = *number,
					.name   = *name,
				};
			}
			default:
				return Cmd::Unknown{
					.message = std::format("Invalid number of arguments: expected one or two, got {}", args.size()),
				};
		}
	}

	struct NameWithExtra {
		std::optional<std::string> name;
		std::optional<std::string> extra;
	};

	auto name_with_extra(std::span<const std::string_view> args) -> std::expected<NameWithExtra, std::string> {
		NameWithExtra result;
		switch (args.size()) {
			case 2: result.extra = std::string{args.front()}; [[fallthrough]];
			case 1: [[fallthrough]];
			case 0: {
				std::expected name = optional_name(args.size() == 2 ? args.last(1) : args);
				if (!name) {
					return std::unexpected(name.error());
				}
				result.name = *name;
				return result;
			}
			default:
				return std::unexpected(std::format("Too many arguments: expected at most two, got {}", args.size()));
		}
	}

	using SubParser = auto (*)(std::span<const std::string_view>) -> Command;
	auto parse_version(std::span<const std::string_view> args) -> Command {
		if (!args.empty()) {
			return Cmd::Unknown{.message = "Doesn't take any arguments"};
		}
		return Cmd::Version{};
	}
	auto parse_list(std::span<const std::string_view> args) -> Command {
		if (args.empty()) {
			return Cmd::Base::List{.gits_only = false};
		}
		if (args.size() == 1 && args.front() == "gits") {
			return Cmd::Base::List{.gits_only = true};
		}
		if (args.size() == 1 && (args.front() == "--names" || args.front() == "-n")) {
			return Cmd::Base::List{.gits_only = false, .names_only = true};
		}
		if (args.size() == 2 && args.front() == "gits" && (args.back() == "--names" || args.back() == "-n")) {
			return Cmd::Base::List{.gits_only = true, .names_only = true};
		}
		return Cmd::Unknown{.message = std::format("Too many arguments: expected at most one, got {}", args.size())};
	}
	auto parse_add(std::span<const std::string_view> args) -> Command {
		const std::expected parts = name_with_extra(args);
		if (!parts || (parts->extra && parts->extra->starts_with('-'))) {
			return Cmd::Unknown{.message = parts ? "Invalid path: must not start with '-'" : parts.error()};
		}
		return Cmd::Base::Add{
			.name = parts->name,
			.path = parts->extra,
		};
	}
	auto parse_remove(std::span<const std::string_view> args) -> Command {
		const std::expected name = optional_name(args);
		if (!name || !*name) {
			return Cmd::Unknown{.message = name ? "Missing project name" : name.error()};
		}
		return Cmd::Base::Remove{.name = **name};
	}
	auto parse_commit(std::span<const std::string_view> args) -> Command {
		const std::expected parts = name_with_extra(args);
		if (!parts) {
			return Cmd::Unknown{.message = parts.error()};
		}
		return Cmd::Git::Commit{
			.name    = parts->name,
			.message = parts->extra,
		};
	}
	auto parse_push(std::span<const std::string_view> args) -> Command {
		const bool          upstream = !args.empty() && args.front() == "-u";
		const std::expected name     = optional_name(upstream ? args.subspan(1) : args);
		if (!name) {
			return Cmd::Unknown{.message = name.error()};
		}
		return Cmd::Git::Push{
			.branch_upstream = upstream,
			.name            = *name,
		};
	}
	auto parse_branch(std::span<const std::string_view> args) -> Command {
		const std::expected parts = name_with_extra(args);
		if (!parts || (parts->extra && parts->extra->starts_with('-'))) {
			return Cmd::Unknown{.message = parts ? "Invalid ref: can't start with '-'" : parts.error()};
		}
		return Cmd::Git::Branch{
			.name        = parts->name,
			.branch_name = parts->extra,
		};
	}
	auto parse_log(std::span<const std::string_view> args) -> Command {
		const bool          oneline = !args.empty() && args.front() == "-o";
		const std::expected name    = optional_name(oneline ? args.subspan(1) : args);
		if (!name) {
			return Cmd::Unknown{.message = name.error()};
		}
		return Cmd::Git::Log{
			.oneline = oneline,
			.name    = *name,
		};
	}
	auto parse_diff(std::span<const std::string_view> args) -> Command {
		const std::expected parts = name_with_extra(args);
		if (!parts || (parts->extra && parts->extra->starts_with('-'))) {
			return Cmd::Unknown{.message = parts ? "Invalid path: must not start with '-'" : parts.error()};
		}
		return Cmd::Git::Diff{
			.name = parts->name,
			.path = parts->extra,
		};
	}
	auto parse_gh(std::span<const std::string_view> args) -> Command {
		if (!args.empty()) {
			return Cmd::Unknown{.message = "Doesn't take any arguments"};
		}
		return Cmd::GH::Status{};
	}
	auto parse_repos(std::span<const std::string_view> args) -> Command {
		if (!args.empty()) {
			return Cmd::Unknown{.message = "Doesn't take any arguments"};
		}
		return Cmd::GH::RepoList{};
	}

	constexpr std::array<std::pair<std::string_view, SubParser>, 37> COMMANDS{
		std::pair{"version", parse_version},
		std::pair{"--version", parse_version},
		std::pair{"-v", parse_version},
		std::pair{"list", parse_list},
		std::pair{"path", parser_with_name<Cmd::Base::Path>},
		std::pair{"add", parse_add},
		std::pair{"remove", parse_remove},
		std::pair{"rm", parse_remove},
		std::pair{"open", parser_with_name<Cmd::Base::Open>},
		std::pair{"build", parser_with_name<Cmd::Base::Build>},
		std::pair{"run", parser_with_name<Cmd::Base::Run>},
		std::pair{"status", with_name_or_all<Cmd::Git::Status>},
		std::pair{"commit", parse_commit},
		std::pair{"push", parse_push},
		std::pair{"pull", with_name_or_all<Cmd::Git::Pull>},
		std::pair{"fetch", with_name_or_all<Cmd::Git::Fetch>},
		std::pair{"branch", parse_branch},
		std::pair{"merge", parser_with_name<Cmd::Git::Merge>},
		std::pair{"log", parse_log},
		std::pair{"diff", parse_diff},
		std::pair{"gh", parse_gh},
		std::pair{"git", parse_gh},
		std::pair{"gits", parse_gh},
		std::pair{"repos", parse_repos},
		std::pair{"ilist", with_name_or_all<Cmd::GH::IssueList>},
		std::pair{"issues", with_name_or_all<Cmd::GH::IssueList>},
		std::pair{"icreate", parser_with_name<Cmd::GH::IssueCreate>},
		std::pair{"iedit", number_with_optional_name<Cmd::GH::IssueEdit>},
		std::pair{"iview", number_with_optional_name<Cmd::GH::IssueView>},
		std::pair{"iclose", number_with_optional_name<Cmd::GH::IssueClose>},
		std::pair{"reqlist", with_name_or_all<Cmd::GH::RequestList>},
		std::pair{"reqs", with_name_or_all<Cmd::GH::RequestList>},
		std::pair{"reqcreate", parser_with_name<Cmd::GH::RequestCreate>},
		std::pair{"reqedit", number_with_optional_name<Cmd::GH::RequestEdit>},
		std::pair{"reqview", number_with_optional_name<Cmd::GH::RequestView>},
		std::pair{"reqclose", number_with_optional_name<Cmd::GH::RequestClose>},
		std::pair{"reqmerge", parser_with_name<Cmd::GH::RequestMerge>},
	};
} // namespace

auto parser(const std::span<const std::string_view> input) -> Command {
	if (input.empty()) {
		return Cmd::Unknown{};
	}

	const auto *const found = std::ranges::find(COMMANDS, input.front(), &std::pair<std::string_view, SubParser>::first);
	if (found != COMMANDS.end()) {
		return found->second(input.subspan(1));
	}

	return Cmd::Unknown{.message = "No command specified (check grammar :-)"};
}
