#include "parser.hpp"
#include <expected>


// grammar: see tests/parser_test.cpp

namespace
{
	// [<name>]: nothing → ".", one word → that word, anything else → nothing
	std::expected<std::optional<std::string>, std::string> optional_name(std::span<const std::string_view> args)
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
		return std::string{args.front()};
	}

	template <typename CommandType>
	Command parser_with_name(std::span<const std::string_view> args)
	{
		const std::expected name = optional_name(args);
		if (!name)
		{
			return Cmd::Unknown{};
		}
		return CommandType{ .name = *name };
	}

	template <typename CommandType>
	Command with_name_or_all(std::span<const std::string_view> args)
	{
		if (args.size() == 1 && (args.front() == "--all" || args.front() == "-a"))
		{
			return CommandType{ .all = true, .name = std::nullopt };
		}
		return parser_with_name<CommandType>(args);
	}

	struct NameWithExtra
	{
		std::optional<std::string> name;
		std::optional<std::string> extra;
	};

	std::expected<NameWithExtra, std::string> name_with_extra(std::span<const std::string_view> args)
	{
		NameWithExtra result;
		switch (args.size())
		{
			case 2:
				result.extra = std::string{ args.front() };
				[[fallthrough]];
			case 1:
				[[fallthrough]];
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
			default:
				return std::unexpected(std::format("Too many arguments: expected at most two, got {}", args.size()));
		}
	}
}


Command parser(const std::span<const std::string_view> input)
{
	if (input.empty())
	{
		return Cmd::Unknown{};
	}

	const std::string_view command = input.front();
	const std::span args = input.subspan(1);

	if (command == "list")
	{
		if (args.empty())
		{
			return Cmd::Base::List{ .gits_only = false };
		}
		if (args.size() == 1 && args.front() == "gits")
		{
			return Cmd::Base::List{ .gits_only = true };
		}
		return Cmd::Unknown{};
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
			return Cmd::Unknown{};
		}
		return Cmd::Base::Add{
			.name	= parts->name,
			.path	= parts->extra,
		};
	}

	if (command == "remove" || command == "rm")
	{
		const std::expected name = optional_name(args);
		if (!name || !*name)
		{
			return Cmd::Unknown{};
		}
		return Cmd::Base::Remove{ .name = **name };
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
			return Cmd::Unknown{ };
		}
		return Cmd::Git::Commit{
			.name		= parts->name,
			.message	= parts->extra,
		};
	}

	if (command == "push")
	{
		const bool upstream = !args.empty() && args.front() == "-u";
		const std::expected name = optional_name(upstream ? args.subspan(1) : args);
		if (!name)
		{
			return Cmd::Unknown{};
		}
		return Cmd::Git::Push{
			.branch_upstream	= upstream,
			.name				= *name,
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
		if (!parts)
		{
			return Cmd::Unknown{};
		}
		return Cmd::Git::Branch{
			.name			= parts->name,
			.branch_name	= parts->extra,
		};
	}

	if (command == "merge")
	{
		return parser_with_name<Cmd::Git::Merge>(args);
	}


	if (command == "git")
	{
		if (!args.empty())
		{
			return Cmd::Unknown{};
		}
		return Cmd::GH::Status{};
	}

	if (command == "repos")
	{
		if (!args.empty())
		{
			return Cmd::Unknown{};
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

	if (command == "reqs" || command == "reqlist")
	{
		return with_name_or_all<Cmd::GH::RequestList>(args);
	}

	if (command == "reqcreate")
	{
		return parser_with_name<Cmd::GH::RequestCreate>(args);
	}

	if (command == "reqmerge")
	{
		return parser_with_name<Cmd::GH::RequestMerge>(args);
	}

	return Cmd::Unknown{};
}
