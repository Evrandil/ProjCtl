#include <print>
#include <string>

#include "projctl.hpp"


namespace {

	enum class CommandOption : uint8_t {
		Path,
		List,
		ListGits,
		GStatus,
		Status,
		Add,
		Remove,
		Open,
		Build,
		Branch,
		Run,
		Commit,
		Push,
		Pull,
		Fetch,
		RepoList,
		IssueList,
		IssueCreate,
		AutoMerge,
		RequestList,
		RequestCreate,
		RequestMerge,
		Idk,
	};

	CommandOption get_option_from_command(const std::string& command) {
		if (command == "list")							{	return CommandOption::List;				}
		if (command == "status")						{	return CommandOption::Status;			}
		if (command == "add")							{	return CommandOption::Add;				}
		if (command == "automerge")						{	return CommandOption::AutoMerge;		}
		if (command == "push")							{	return CommandOption::Push;				}
		if (command == "path")							{	return CommandOption::Path;				}
		if (command == "pull")							{	return CommandOption::Pull;				}
		if (command == "repos")							{	return CommandOption::RepoList;			}
		if (command == "run")							{	return CommandOption::Run;				}
		if (command == "rm" || command == "remove")		{	return CommandOption::Remove;			}
		if (command == "open")							{	return CommandOption::Open;				}
		if (command == "branch")						{	return CommandOption::Branch;			}
		if (command == "build")							{	return CommandOption::Build;			}
		if (command == "commit")						{	return CommandOption::Commit;			}
		if (command == "fetch")							{	return CommandOption::Fetch;			}
		if (command == "git")							{	return CommandOption::GStatus;			}
		if (command == "issues" || command == "ilist")	{	return CommandOption::IssueList;		}
		if (command == "icreate")						{	return CommandOption::IssueCreate;		}
		if (command == "reqs" || command == "reqlist")	{	return CommandOption::RequestList;		}
		if (command == "reqcreate")						{	return CommandOption::RequestCreate;	}
		if (command == "reqmerge")						{	return CommandOption::RequestMerge;		}
		return CommandOption::Idk;
	}

}

int main(const int arg_count, const char **argv) {
	ProjCtl proj_ctl;
	const size_t argc = static_cast<size_t>(arg_count);
	std::vector<std::string> args(argv, argv + argc);
	size_t arg_iterator = 1;

	std::string command = argv[arg_iterator++];

	CommandOption command_option = get_option_from_command(command);


	if (command_option == CommandOption::Path) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.path_show(std::string { argv[arg_iterator++] } );
		return 0;
	}	std::println("\n\n");


	if (command_option == CommandOption::GStatus) {
		proj_ctl.g_status();
		goto finish;
	}


	if (command_option == CommandOption::RepoList) {
		proj_ctl.repo_list();
		goto finish;
	}


	if (command_option == CommandOption::List) {
		if (arg_iterator < argc && args.at(arg_iterator) == "gits") {
			proj_ctl.list_gits();
		} else {
			proj_ctl.list();
		}
		goto finish;
	}


	if (command_option == CommandOption::Status) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.status(args.at(arg_iterator));
		goto finish;
	}

	if (command_option == CommandOption::Add) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		if (args.at(arg_iterator) == ".") {
			++arg_iterator;
			proj_ctl.add_current();
			return 1;
		}
		if (arg_iterator + 2 > argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		const std::string& name = args.at(arg_iterator++);
		std::filesystem::path path = args.at(arg_iterator++);
		proj_ctl.add(name, path);
		goto finish;
	}


	if (command_option == CommandOption::Remove) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.remove(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::Open) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.open(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::Build) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.build(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::Run) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.run(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::Commit) {
		bool push = false;
		bool branch = false;
		if (arg_iterator < argc && args.at(arg_iterator) == "push") {
			push = true;
			++arg_iterator;
			if (arg_iterator < argc && args.at(arg_iterator) == "branch") {
				branch = true;
				++arg_iterator;
			}
		}
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		const std::string& name = args.at(arg_iterator++);
		if (arg_iterator == argc) {
			proj_ctl.git_commit(name);
		}
		else {
			std::string_view message = args.at(arg_iterator++);
			proj_ctl.git_commit(name, message);
		}
		if (push) {
			if (branch) {
				proj_ctl.git_push_branch(name);
			}
			else {
				proj_ctl.git_push(name);
			}
		}
		goto finish;
	}


	if (command_option == CommandOption::AutoMerge) {
		if (arg_iterator + 1 >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		const std::string& name = args.at(arg_iterator++);
		std::string_view message = args.at(arg_iterator++);
		proj_ctl.git_automerge(name, message);
		goto finish;
	}


	if (command_option == CommandOption::Push) {
		if (arg_iterator < argc && args.at(arg_iterator) == "branch") {
			++arg_iterator;
			proj_ctl.git_push_branch(args.at(arg_iterator++));
			goto finish;
		}
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.git_push(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::Pull) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.git_pull(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::Branch) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		if (arg_iterator + 1 == argc) {
			proj_ctl.git_branch_list(args.at(arg_iterator++));
			goto finish;
		}
		const std::string& name = args.at(arg_iterator++);
		std::string_view branch = args.at(arg_iterator++);
		proj_ctl.git_branch(name, branch);
		goto finish;
	}


	if (command_option == CommandOption::Fetch) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.git_fetch(args.at(arg_iterator++));
		goto finish;
	}




	if (command_option == CommandOption::IssueList) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.issue_list(args.at(arg_iterator++));
		goto finish;
	}

	if (command_option == CommandOption::IssueCreate) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.issue_create(args.at(arg_iterator++));
		goto finish;
	}


	if (command_option == CommandOption::RequestList) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments");
			return 1;
		}
		proj_ctl.request_list(args.at(arg_iterator++));
		goto finish;
	}

	if (command_option == CommandOption::RequestCreate) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments!");
			return 1;
		}
		proj_ctl.request_create(args.at(arg_iterator++));
		goto finish;
	}

	if (command_option == CommandOption::RequestMerge) {
		if (arg_iterator >= argc) {
			std::println("You're missing some arguments");
			return 1;
		}
		proj_ctl.request_merge(args.at(arg_iterator++));
		goto finish;
	}

	std::println("Unrecognized command");
	return 1;

finish:
	std::print("\n\n");
	return 0;
}
