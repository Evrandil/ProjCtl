#include "parser.hpp"
#include <charconv>
#include <expected>

// grammar: see tests/parser_test.cpp

namespace
{
	auto optional_name(std::span<const std::string_view> args) -> std::expected<std::optional<std::string>, std::string>
	{
		if (args.empty() || (args.size() == 1 && args.front() == "."))
		{
			return std::nullopt;
		}
		if (args.size() > 1)
		{
			return std::unexpected(std::format("Too many arguments: expected at most one project name, got {}", args.size()));
		}
		if (args.front().starts_with('-'))
		{
			return std::unexpected(std::format("Unknown option: {}", args.front()));
		}
		if (args.front().contains('/'))
		{
			return std::unexpected(std::format("Invalid project name: {}", args.front()));
		}
		return std::string{args.front()};
	}

	template <typename CommandType>
	auto parser_with_name(std::span<const std::string_view> args) -> Command
	{
		const std::expected name = optional_name(args);
		if (!name)
		{
			return Cmd::Unknown{.message = name.error()};
		}
		return CommandType{.name = *name};
	}

	template <typename CommandType>
	auto with_name_or_all(std::span<const std::string_view> args) -> Command
	{
		if (args.size() == 1 && (args.front() == "--all" || args.front() == "-a"))
		{
			return CommandType{.all = true, .name = std::nullopt};
		}
		return parser_with_name<CommandType>(args);
	}

	auto parse_number(std::string_view text) -> std::optional<int>
	{
		int value               = 0;
		const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
		if (error != std::errc{} || end != text.data() + text.size() || value <= 0)
		{
			return std::nullopt;
		}
		return value;
	}

	template <typename CommandType>
	auto number_with_optional_name(std::span<const std::string_view> args) -> Command
	{
		std::expected<std::optional<std::string>, std::string> name = std::nullopt;
		switch (args.size())
		{
			case 2:
				name = optional_name(args.last(1));
				if (!name)
				{
					return Cmd::Unknown{.message = name.error()};
				}
				[[fallthrough]];
			case 1:
			{
				std::optional<int> number = parse_number(args.front());
				if (!number)
				{
					return Cmd::Unknown{.message = std::format("Invalid number: {}", args.front())};
				}

				return CommandType{
					.number = *number,
					.name   = *name,
				};
			}
			default: return Cmd::Unknown{.message = std::format("Invalid number of arguments: expected one or two, got {}", args.size())};
		}
	}

	struct NameWithExtra
	{
		std::optional<std::string> name;
		std::optional<std::string> extra;
	};

	auto name_with_extra(std::span<const std::string_view> args) -> std::expected<NameWithExtra, std::string>
	{
		NameWithExtra result;
		switch (args.size())
		{
			case 2: result.extra = std::string{args.front()}; [[fallthrough]];
			case 1: [[fallthrough]];
			case 0:
			{
				std::expected name = optional_name(args.size() == 2 ? args.last(1) : args);
				if (!name)
				{
					return std::unexpected(name.error());
				}
				result.name = *name;
				return result;
			}
			default: return std::unexpected(std::format("Too many arguments: expected at most two, got {}", args.size()));
		}
	}
} // namespace

auto parser(const std::span<const std::string_view> input) -> Command
{
	if (input.empty())
	{
		return Cmd::Unknown{};
	}

	const std::string_view command = input.front();
	const std::span        args    = input.subspan(1);

	if (command == "version" || command == "-v" || command == "--version")
	{
		if (!args.empty())
		{
			return Cmd::Unknown{.message = "Doesn't take any arguments"};
		}
		return Cmd::Version{};
	}

	if (command == "list")
	{
		if (args.empty())
		{
			return Cmd::Base::List{.gits_only = false};
		}
		if (args.size() == 1 && args.front() == "gits")
		{
			return Cmd::Base::List{.gits_only = true};
		}
		if (args.size() == 1 && (args.front() == "--names" || args.front() == "-n"))
		{
			return Cmd::Base::List{.gits_only = false, .names_only = true};
		}
		if (args.size() == 2 && args.front() == "gits" && (args.back() == "--names" || args.back() == "-n"))
		{
			return Cmd::Base::List{.gits_only = true, .names_only = true};
		}
		return Cmd::Unknown{.message = std::format("Too many arguments: expected at most one, got {}", args.size())};
	}

	if (command == "path")
	{
		return parser_with_name<Cmd::Base::Path>(args);
	}

	if (command == "add")
	{
		const std::expected parts = name_with_extra(args);
		if (!parts || (parts->extra && parts->extra->starts_with('-')))
		{
			return Cmd::Unknown{.message = parts ? "Invalid path: must not start with '-'" : parts.error()};
		}
		return Cmd::Base::Add{
			.name = parts->name,
			.path = parts->extra,
		};
	}

	if (command == "remove" || command == "rm")
	{
		const std::expected name = optional_name(args);
		if (!name || !*name)
		{
			return Cmd::Unknown{.message = name ? "Missing project name" : name.error()};
		}
		return Cmd::Base::Remove{.name = **name};
	}

	if (command == "open")
	{
		return parser_with_name<Cmd::Base::Open>(args);
	}

	if (command == "build")
	{
		return parser_with_name<Cmd::Base::Build>(args);
	}

	if (command == "run")
	{
		return parser_with_name<Cmd::Base::Run>(args);
	}

	if (command == "status")
	{
		return with_name_or_all<Cmd::Git::Status>(args);
	}

	if (command == "commit")
	{
		const std::expected parts = name_with_extra(args);
		if (!parts)
		{
			return Cmd::Unknown{.message = parts.error()};
		}
		return Cmd::Git::Commit{
			.name    = parts->name,
			.message = parts->extra,
		};
	}

	if (command == "push")
	{
		const bool          upstream = !args.empty() && args.front() == "-u";
		const std::expected name     = optional_name(upstream ? args.subspan(1) : args);
		if (!name)
		{
			return Cmd::Unknown{.message = name.error()};
		}
		return Cmd::Git::Push{
			.branch_upstream = upstream,
			.name            = *name,
		};
	}

	if (command == "pull")
	{
		return with_name_or_all<Cmd::Git::Pull>(args);
	}

	if (command == "fetch")
	{
		return with_name_or_all<Cmd::Git::Fetch>(args);
	}

	if (command == "branch")
	{
		const std::expected parts = name_with_extra(args);
		if (!parts || (parts->extra && parts->extra->starts_with('-')))
		{
			return Cmd::Unknown{.message = parts ? "Invalid ref: can't start with '-'" : parts.error()};
		}
		return Cmd::Git::Branch{
			.name        = parts->name,
			.branch_name = parts->extra,
		};
	}

	if (command == "merge")
	{
		return parser_with_name<Cmd::Git::Merge>(args);
	}

	if (command == "log")
	{
		const bool          oneline = !args.empty() && args.front() == "-o";
		const std::expected name    = optional_name(oneline ? args.subspan(1) : args);
		if (!name)
		{
			return Cmd::Unknown{.message = name.error()};
		}
		return Cmd::Git::Log{
			.oneline = oneline,
			.name    = *name,
		};
	}

	if (command == "diff")
	{
		const std::expected parts = name_with_extra(args);
		if (!parts || (parts->extra && parts->extra->starts_with('-')))
		{
			return Cmd::Unknown{.message = parts ? "Invalid path: must not start with '-'" : parts.error()};
		}
		return Cmd::Git::Diff{
			.name = parts->name,
			.path = parts->extra,
		};
	}

	if (command == "git" || command == "gits" || command == "gh")
	{
		if (!args.empty())
		{
			return Cmd::Unknown{.message = "Doesn't take any arguments"};
		}
		return Cmd::GH::Status{};
	}

	if (command == "repos")
	{
		if (!args.empty())
		{
			return Cmd::Unknown{.message = "Doesn't take any arguments"};
		}
		return Cmd::GH::RepoList{};
	}

	if (command == "issues" || command == "ilist")
	{
		return with_name_or_all<Cmd::GH::IssueList>(args);
	}

	if (command == "icreate")
	{
		return parser_with_name<Cmd::GH::IssueCreate>(args);
	}

	if (command == "iedit")
	{
		return number_with_optional_name<Cmd::GH::IssueEdit>(args);
	}

	if (command == "iview")
	{
		return number_with_optional_name<Cmd::GH::IssueView>(args);
	}

	if (command == "iclose")
	{
		return number_with_optional_name<Cmd::GH::IssueClose>(args);
	}

	if (command == "reqs" || command == "reqlist")
	{
		return with_name_or_all<Cmd::GH::RequestList>(args);
	}

	if (command == "reqcreate")
	{
		return parser_with_name<Cmd::GH::RequestCreate>(args);
	}

	if (command == "reqedit")
	{
		return number_with_optional_name<Cmd::GH::RequestEdit>(args);
	}

	if (command == "reqview")
	{
		return number_with_optional_name<Cmd::GH::RequestView>(args);
	}

	if (command == "reqclose")
	{
		return number_with_optional_name<Cmd::GH::RequestClose>(args);
	}

	if (command == "reqmerge")
	{
		return parser_with_name<Cmd::GH::RequestMerge>(args);
	}

	return Cmd::Unknown{.message = "No command specified (check grammar :-)"};
}
