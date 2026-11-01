#include <print>
#include <string>
#include <expected>

#include "projctl.hpp"


enum class CommandOption : uint8_t {
	Path,
	List,
	GStatus,
	RepoList,
	Status,
	Add,
	Remove,
	Open,
	Build,
	Run,
	Pull,
	Fetch,
	Branch,
	Commit,
	Push,
	AutoMerge,
	IssueList,
	IssueCreate,
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


// returns error = false if command was path and was handled, true if command was not handled, nothing if everything was handled successfully
std::expected<void, bool> deal_with_it(std::vector<std::string>&args, ProjCtl& proj_ctl) {

	std::vector<std::string>::iterator arg_iterator = args.begin();
	CommandOption command_option = get_option_from_command(*arg_iterator++);
	const std::string& name = *args.rbegin();


	if (command_option != CommandOption::Path) {
		std::println("\n\n");
	}

	// Let's get rid of commands that don't require any arguments
	switch (command_option) {

		case CommandOption::GStatus:
			{
				proj_ctl.g_status();
				return {};
			}

		case CommandOption::RepoList:
			{
				proj_ctl.repo_list();
				return {};
			}

		case CommandOption::List:
			{
				if (arg_iterator < args.end() && *arg_iterator == "gits") {
					proj_ctl.list_gits();
				} else {
					proj_ctl.list();
				}
				return {};
			}

		case CommandOption::Idk:
			{
				std::println("Unrecognized command");
				return std::unexpected(true);
			}


			// Rest of commands require at least one argument (project name for most)
		default:
			{
				if (arg_iterator < args.end()) {
					std::println("Unrecognized command");
					return std::unexpected(true);
				}
				[[fallthrough]];
			}


		case CommandOption::Path:
			{
				proj_ctl.path_show(name);
				return std::unexpected(false);
			}

		case CommandOption::Status:
			{
				proj_ctl.status(name);
				return {};
			}

		case CommandOption::Remove:
			{
				proj_ctl.remove(name);
				return {};
			}

		case CommandOption::Open:
			{
				proj_ctl.open(name);
				return {};
			}

		case CommandOption::Build:
			{
				while (arg_iterator < args.end()) {
					proj_ctl.build(*arg_iterator++);
				}
				return {};
			}

		case CommandOption::Run:
			{
				proj_ctl.run(name);
				return {};
			}

		case CommandOption::Add:
			{
				if (*arg_iterator == ".") {
					proj_ctl.add_current();
					return {};
				}
				const std::string& add_name = *arg_iterator++;
				if (arg_iterator >= args.end()) {
					std::println("You're missing some arguments!");
					return std::unexpected(true);
				}
				proj_ctl.add(add_name, *arg_iterator);
				return {};
			}


		// GitHub/GitLab functionality
		case CommandOption::IssueList:
			{
				proj_ctl.issue_list(name);
				return {};
			}

		case CommandOption::IssueCreate:
			{
				proj_ctl.issue_create(name);
				return {};
			}

		case CommandOption::RequestList:
			{
				proj_ctl.request_list(name);
				return {};
			}

		case CommandOption::RequestCreate:
			{
				proj_ctl.request_create(name);
				return {};
			}

		case CommandOption::RequestMerge:
			{
				proj_ctl.request_merge(name);
				return {};
			}


		// Git functionality
		case CommandOption::Fetch:
			{
				proj_ctl.git_fetch(name);
				return {};
			}

		case CommandOption::Branch:
			{
				if (arg_iterator + 1 == args.end()) {
					proj_ctl.git_branch_list(name);
					return {};
				}
				std::string_view branch = *arg_iterator;
				proj_ctl.git_branch(name, branch);
				return {};
			}

		case CommandOption::Pull:
			{
				proj_ctl.git_pull(name);

				if (*arg_iterator == "automerge")		{	command_option = CommandOption::AutoMerge;	}
				else if (*arg_iterator == "commit")		{	command_option = CommandOption::Commit;		}
				else if (*arg_iterator == "push")		{	command_option = CommandOption::Push;		}
				[[fallthrough]];
			}
		case CommandOption::AutoMerge:
			{
				proj_ctl.git_automerge(name);

				if (*arg_iterator == "commit")			{	command_option = CommandOption::Commit;		}
				else if (*arg_iterator == "push")		{	command_option = CommandOption::Push;		}
				arg_iterator++;
				[[fallthrough]];
			}
		case CommandOption::Commit:
			{
				if (*arg_iterator == "push" || *arg_iterator == name) {
					proj_ctl.git_commit(name);
				} else {
					proj_ctl.git_commit(name, *arg_iterator);
				}
				arg_iterator++;

				if (*arg_iterator == "push") {
					command_option = CommandOption::Push;
				}
				[[fallthrough]];
			}
		case CommandOption::Push:
			{
				if (*arg_iterator == "branch") {
					proj_ctl.git_push_branch(name);
					return {};
				}
				proj_ctl.git_push(name);
			}
	}
	return {};
}


int main(const int arg_count, const char **argv) {
	ProjCtl proj_ctl;
	const size_t argc = static_cast<size_t>(arg_count);
	std::vector<std::string> args { argv + 1, argv + argc } ;

	std::expected<void, bool> result = deal_with_it(args, proj_ctl);

	if (result || result.error()) {
		std::print("\n\n");
		return 0;
	}
}
