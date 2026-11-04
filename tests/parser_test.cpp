#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "parser.hpp"

#include <string_view>
#include <variant>
#include <vector>

namespace
{
	Command parse(std::vector<std::string_view> args) { return parser(args); }
}


// projctl list [gits]
TEST_CASE("list")
{
	// projctl list
	SUBCASE("plain list")
	{
		const Command command = parse({"list"});
		const auto* list = std::get_if<Cmd::Base::List>(&command);
		REQUIRE(list != nullptr);
		CHECK_FALSE(list->gits_only);
	}
	// projctl list gits
	SUBCASE("list gits")
	{
		const Command command = parse({"list", "gits"});
		const auto* list = std::get_if<Cmd::Base::List>(&command);
		REQUIRE(list != nullptr);
		CHECK(list->gits_only);
	}
}
// projctl add <path> <name>
TEST_CASE("add")
{
	// projctl add [path] [name]
	SUBCASE("add with path and name")
	{
		const Command command = parse({"add", "/path/to/project", "proj"});
		const auto* add = std::get_if<Cmd::Base::Add>(&command);
		REQUIRE(add != nullptr);
		CHECK(add->name == "proj");
		CHECK(add->path == "/path/to/project");
	}
	// projctl add . [name]
	SUBCASE("add current directory with name")
	{
		const Command command = parse({"add", ".", "proj"});
		const auto* add = std::get_if<Cmd::Base::Add>(&command);
		REQUIRE(add != nullptr);
		CHECK(add->name == "proj");
		CHECK(add->path == ".");
	}
}
// projctl remove <name>
// projctl rm <name>
TEST_CASE("remove")
{
	// projctl remove [name]
	SUBCASE("plain remove")
	{
		const Command command = parse({"remove", "proj"});
		const auto* remove = std::get_if<Cmd::Base::Remove>(&command);
		REQUIRE(remove != nullptr);
		CHECK(remove->name == "proj");
	}
	// projctl rm [name]
	SUBCASE("rm alias")
	{
		const Command command = parse({"rm", "proj"});
		const auto* remove = std::get_if<Cmd::Base::Remove>(&command);
		REQUIRE(remove != nullptr);
		CHECK(remove->name == "proj");
	}
}
// projctl path [<name>]
TEST_CASE("path")
{
	const Command command = parse({"path", "proj"});
	const auto* path = std::get_if<Cmd::Base::Path>(&command);
	REQUIRE(path != nullptr);
	CHECK(path->name == "proj");
}
// projctl open [<name>]
TEST_CASE("open")
{
	const Command command = parse({"open", "proj"});
	const auto* open = std::get_if<Cmd::Base::Open>(&command);
	REQUIRE(open != nullptr);
	CHECK(open->name == "proj");
}
// projctl build [<name>]
TEST_CASE("build")
{
	const Command command = parse({"build", "proj"});
	const auto* build = std::get_if<Cmd::Base::Build>(&command);
	REQUIRE(build != nullptr);
	CHECK(build->name == "proj");
}
// projctl run [<name>]
TEST_CASE("run")
{
	const Command command = parse({"run", "proj"});
	const auto* run = std::get_if<Cmd::Base::Run>(&command);
	REQUIRE(run != nullptr);
	CHECK(run->name == "proj");
}
// projctl branch [<branch>] <name>
TEST_CASE("branch")
{
	// projctl branch <name>
	SUBCASE("plain branch")
	{
		const Command command = parse({"branch", "proj"});
		const auto* branch = std::get_if<Cmd::Git::Branch>(&command);
		REQUIRE(branch != nullptr);
		CHECK(branch->name == "proj");
		CHECK_FALSE(branch->branch_name);
	}
	// projctl branch <branch_name> <name>
	SUBCASE("branch with name")
	{
		const Command command = parse({"branch", "feature", "proj"});
		const auto* branch = std::get_if<Cmd::Git::Branch>(&command);
		REQUIRE(branch != nullptr);
		CHECK(branch->name == "proj");
		REQUIRE(branch->branch_name);
		CHECK(*branch->branch_name == "feature");
	}
}
// projctl status [<name> | --all | -a]
TEST_CASE("status")
{
	// projctl status <name>
	SUBCASE("plain status")
	{
		const Command command = parse({"status", "proj"});
		const auto* status = std::get_if<Cmd::Git::Status>(&command);
		REQUIRE(status != nullptr);
		CHECK(status->name == "proj");
	}
	// projctl status --all | -a
	SUBCASE("status all")
	{
		const Command command = parse({"status", "--all"});
		const auto* status = std::get_if<Cmd::Git::Status>(&command);
		REQUIRE(status != nullptr);
		CHECK(status->name == "--all");
	}
}
//projctl fetch [<name> | --all | -a]
TEST_CASE("fetch")
{
	// projctl fetch <name>
	SUBCASE("plain fetch")
	{
		const Command command = parse({"fetch", "proj"});
		const auto* fetch = std::get_if<Cmd::Git::Fetch>(&command);
		REQUIRE(fetch != nullptr);
		CHECK(fetch->name == "proj");
	}
	// projctl fetch --all | -a
	SUBCASE("fetch all")
	{
		const Command command = parse({"fetch", "--all"});
		const auto* fetch = std::get_if<Cmd::Git::Fetch>(&command);
		REQUIRE(fetch != nullptr);
		CHECK(fetch->name == "--all");
	}
}
// projctl pull [<name> | --all | -a]
TEST_CASE("pull")
{
	// projctl pull <name>
	SUBCASE("plain pull")
	{
		const Command command = parse({"pull", "proj"});
		const auto* pull = std::get_if<Cmd::Git::Pull>(&command);
		REQUIRE(pull != nullptr);
		CHECK(pull->name == "proj");
	}
	// projctl pull --all | -a
	SUBCASE("pull all")
	{
		const Command command = parse({"pull", "--all"});
		const auto* pull = std::get_if<Cmd::Git::Pull>(&command);
		REQUIRE(pull != nullptr);
		CHECK(pull->name == "--all");
	}
}
// projctl merge [<name>]
TEST_CASE("merge")
{
	const Command command = parse({"merge", "proj"});
	const auto* automerge = std::get_if<Cmd::Git::Merge>(&command);
	REQUIRE(automerge != nullptr);
	CHECK(automerge->name == "proj");
}
// projctl commit [[<message>] <name>]
TEST_CASE("commit")
{
	// projctl commit <name>
	SUBCASE("plain commit")
	{
		const Command command = parse({"commit", "proj"});
		const auto* commit = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		CHECK(commit->name == "proj");
		CHECK_FALSE(commit->message);
	}
	// projctl commit <commit_message> <name>
	SUBCASE("commit with message")
	{
		const Command command = parse({"commit", "Fix bug", "proj"});
		const auto* commit = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		CHECK(commit->name == "proj");
		REQUIRE(commit->message);
		CHECK(*commit->message == "Fix bug");
	}
}
// projctl push [-u] [<name>]
TEST_CASE("push")
{
	// projctl push <name>
	SUBCASE("plain push")
	{
		const Command command = parse({"push", "proj"});
		const auto* push = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		CHECK(push->name == "proj");
		CHECK_FALSE(push->branch_upstream);
	}
	// projctl push -u <name>
	SUBCASE("push upstream")
	{
		const Command command = parse({"push", "-u", "proj"});
		const auto* push = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		CHECK(push->branch_upstream);
	}
}
/*
For later to implement (git pipeling):
projctl pull automerge commit push [name]
projctl pull automerge commit [commit_msg] push [name]
projctl pull automerge commit push branch [name]
projctl pull automerge commit [commit_message] push branch [name]
*/

// projctl git | repos
TEST_CASE("gh commands")
{
	// projctl git
	SUBCASE("gh status")
	{
		const Command command = parse({"git"});
		const auto* gh_status = std::get_if<Cmd::GH::Status>(&command);
		REQUIRE(gh_status != nullptr);
	}
	// projctl repos
	SUBCASE("gh repos")
	{
		const Command command = parse({"repos"});
		const auto* gh_repos = std::get_if<Cmd::GH::RepoList>(&command);
		REQUIRE(gh_repos != nullptr);
	}
}


// projctl ilist [<name> | --all | -a]
// projctl icreate [<name> | --all | -a]
TEST_CASE("gh issue commands")
{
	// projctl ilist <name>
	SUBCASE("gh issue list")
	{
		const Command command = parse({"ilist", "proj"});
		const auto* gh_issue_list = std::get_if<Cmd::GH::IssueList>(&command);
		REQUIRE(gh_issue_list != nullptr);
		CHECK(gh_issue_list->name == "proj");
	}
	// projctl icreate <name>
	SUBCASE("gh issue create")
	{
		const Command command = parse({"icreate", "proj"});
		const auto* gh_issue_create = std::get_if<Cmd::GH::IssueCreate>(&command);
		REQUIRE(gh_issue_create != nullptr);
		CHECK(gh_issue_create->name == "proj");
	}
}
// projctl reqlist [<name>]
// projctl reqcreate [<name>]
// projctl reqcreate [<name>]
TEST_CASE("gh request commands")
{
	// projctl reqlist <name>
	SUBCASE("gh request list")
	{
		const Command command = parse({"reqlist", "proj"});
		const auto* gh_request_list = std::get_if<Cmd::GH::RequestList>(&command);
		REQUIRE(gh_request_list != nullptr);
		CHECK(gh_request_list->name == "proj");
	}
	// projctl reqcreate <name>
	SUBCASE("gh request create")
	{
		const Command command = parse({"reqcreate", "proj"});
		const auto* gh_request_create = std::get_if<Cmd::GH::RequestCreate>(&command);
		REQUIRE(gh_request_create != nullptr);
		CHECK(gh_request_create->name == "proj");
	}
	//projctl reqmerge <name>
	SUBCASE("gh request merge")
	{
		const Command command = parse({"reqmerge", "proj"});
		const auto* gh_request_merge = std::get_if<Cmd::GH::RequestMerge>(&command);
		REQUIRE(gh_request_merge != nullptr);
		CHECK(gh_request_merge->name == "proj");
	}
}


TEST_CASE("bad input is rejected")
{
	CHECK(std::holds_alternative<Cmd::Unknown>(parse({})));
	CHECK(std::holds_alternative<Cmd::Unknown>(parse({"remove", "a", "b"})));
	CHECK(std::holds_alternative<Cmd::Unknown>(parse({"nonsense", "proj"})));
}
