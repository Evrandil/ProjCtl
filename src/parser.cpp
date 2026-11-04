#include "parser.hpp"


/*
projctl list [gits]
projctl status [<name> | --all | -a]
projctl add <path> <name>
projctl remove <name>
projctl rm <name>
projctl path [<name>]
projctl open [<name>]
projctl build [<name>]
projctl run [<name>]
projctl branch [<branch>] <name>
projctl fetch [<name> | --all | -a]
projctl pull [<name> | --all | -a]
projctl merge [<name>]
projctl commit [[<message>] <name>]
projctl push [-u] [<name>]
projctl git
projctl repos
projctl ilist [<name> | --all | -a]
projctl icreate [<name> | --all | -a]
projctl reqlist [<name>]
projctl reqcreate [<name>]
projctl reqcreate [<name>]
*/


Command parser(std::span<const std::string_view> args)
{
	if (args.empty())
	{
		return Cmd::Unknown{};
	}

	const std::string_view command = args.at(0);

	if (command == "git")
	{
		return Cmd::GH::Status{};
	}

	if (command == "repos")
	{
		return Cmd::GH::RepoList{};
	}


	if (command == "list")
	{
		try
		{
			if (args.at(1) == "gits")
			{
				return Cmd::Base::List{true};
			}
		}
		catch (const std::out_of_range& err)
		{
			return Cmd::Base::List{false};
		}
		return Cmd::Unknown{};
	}


	std::string name;

	if (args.size() < 2)
	{
		name = ".";
	}
	else
	{
		name = std::string{*args.rbegin()};
	}


	if (command == "path")
	{
		return Cmd::Base::Path{name};
	}

	if (command == "add")
	{
		if (name == ".")
		{
			return Cmd::Base::Add{
				name,
				std::filesystem::path{"."},
			};
		}
		if (args.size() < 3)
		{
			return Cmd::Unknown{};
		}
		return Cmd::Base::Add{std::string{args.at(1)}, std::filesystem::path{args.at(2)}};
	}

	if (command == "remove" || command == "rm")
	{
		return Cmd::Base::Remove{name};
	}

	if (command == "open")
	{
		return Cmd::Base::Open{name};
	}

	if (command == "build")
	{
		return Cmd::Base::Build{name};
	}

	if (command == "run")
	{
		return Cmd::Base::Run{name};
	}


	if (command == "status")
	{
		return Cmd::Git::Status{name};
	}

	if (command == "commit")
	{
		if (args.size() < 3)
		{
			return Cmd::Git::Commit{
				name,
				std::nullopt,
			};
		}
		return Cmd::Git::Commit{
			name,
			std::string{args.at(1)},
		};
	}

	if (command == "push")
	{
		try
		{
			if (args.at(1) == "branch")
			{
				return Cmd::Git::Push{
					name,
					true,
				};
			}
		}
		catch (const std::out_of_range& err)
		{
			return Cmd::Git::Push{
				name,
				false,
			};
		}
	}

	if (command == "pull")
	{
		return Cmd::Git::Pull{name};
	}

	if (command == "fetch")
	{
		return Cmd::Git::Fetch{name};
	}

	if (command == "branch")
	{
		if (args.size() < 3)
		{
			return Cmd::Git::Branch{
				name,
				std::nullopt,
			};
		}
		return Cmd::Git::Branch{
			name,
			std::string{args.at(1)},
		};
	}


	if (command == "merge")
	{
		return Cmd::Git::Merge{name};
	}

	if (command == "issues" || command == "ilist")
	{
		return Cmd::GH::IssueList{name};
	}

	if (command == "icreate")
	{
		return Cmd::GH::IssueCreate{name};
	}


	if (command == "reqs" || command == "reqlist")
	{
		return Cmd::GH::RequestList{name};
	}

	if (command == "reqcreate")
	{
		return Cmd::GH::RequestCreate{name};
	}

	if (command == "reqmerge")
	{
		return Cmd::GH::RequestMerge{name};
	}

	return Cmd::Unknown{};
}
