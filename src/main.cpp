#include "projctl.hpp"
#include "parser.hpp"
#include <print>
#include <string>
#include <string_view>
#include <vector>


/*
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
	Fetch,
	Branch,
	GitPipeline,
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
	if (command == "automerge")						{	return CommandOption::GitPipeline;		}
	if (command == "push")							{	return CommandOption::GitPipeline;		}
	if (command == "path")							{	return CommandOption::Path;				}
	if (command == "pull")							{	return CommandOption::GitPipeline;		}
	if (command == "repos")							{	return CommandOption::RepoList;			}
	if (command == "run")							{	return CommandOption::Run;				}
	if (command == "rm" || command == "remove")		{	return CommandOption::Remove;			}
	if (command == "open")							{	return CommandOption::Open;				}
	if (command == "branch")						{	return CommandOption::Branch;			}
	if (command == "build")							{	return CommandOption::Build;			}
	if (command == "commit")						{	return CommandOption::GitPipeline;		}
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

	// TODO config management

	if (args.empty()) {
		std::println("No command given");
		return std::unexpected(true);
	}

	std::vector<std::string>::iterator arg_iterator = args.begin();
	CommandOption command_option = get_option_from_command(*arg_iterator++);


	if (command_option != CommandOption::Path) {
		std::println("\n\n");
	}

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

		default:
			{
				if (arg_iterator >= args.end()) {
					std::println("You're missing some arguments!");
					return std::unexpected(true);
				}
			}

	}


	const std::string& name = *args.rbegin();


	switch (command_option) {

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

		case CommandOption::GitPipeline:
			{
				arg_iterator--;
				auto take = [&] (std::string_view word) {
					if (arg_iterator < args.end() && *arg_iterator == word) {
						arg_iterator++;
						return true;
					}
					return false;
				};

				const bool do_pull = take("pull");
				const bool do_automerge = take("automerge");
				const bool do_commit = take("commit");
				const bool do_push = take("push");
				const bool push_branch = do_push && take("branch");

				if (do_pull) {
					proj_ctl.git_pull(name);
				}
				if (do_automerge) {
					proj_ctl.git_automerge(name);
				}
				if (do_commit) {
					if (*arg_iterator == "push" || *arg_iterator == name) {
						proj_ctl.git_commit(name);
					} else {
						proj_ctl.git_commit(name, *arg_iterator++);
					}
				}
				if (do_push) {
					if (push_branch) {
						proj_ctl.git_push_branch(name);
						return {};
					}
					proj_ctl.git_push(name);
				}
				return {};
			}


		default:
			{
				std::println("Unrecognized command");
				return std::unexpected(true);
			}

	}
}*/


int main(const int argc, const char **argv) {
	std::vector<std::string_view> args { argv + 1, argv + argc } ;

	ProjCtl proj_ctl;

	return proj_ctl.execute(parser(args));
}
