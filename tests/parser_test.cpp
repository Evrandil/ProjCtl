#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "parser.hpp"

#include <string_view>
#include <variant>
#include <vector>

namespace {
	Command parse(std::vector<std::string_view> args) {
		return parser(args);
	}
} // namespace

// projctl list [gits]
TEST_CASE("list") {
	// projctl list
	SUBCASE("plain list") {
		const Command command = parse({"list"});
		const auto*   list    = std::get_if<Cmd::Base::List>(&command);
		REQUIRE(list != nullptr);
		CHECK_FALSE(list->gits_only);
	}
	// projctl list gits
	SUBCASE("list gits") {
		const Command command = parse({"list", "gits"});
		const auto*   list    = std::get_if<Cmd::Base::List>(&command);
		REQUIRE(list != nullptr);
		CHECK(list->gits_only);
	}
}

// projctl add [[<path>] <name>]
TEST_CASE("add") {
	// projctl add
	SUBCASE("add current directory with no name") {
		const Command command = parse({"add"});
		const auto*   add     = std::get_if<Cmd::Base::Add>(&command);
		REQUIRE(add != nullptr);
		CHECK_FALSE(add->name);
		CHECK_FALSE(add->path);
	}
	// projctl add <name>
	SUBCASE("add current directory with name") {
		const Command command = parse({"add", "proj"});
		const auto*   add     = std::get_if<Cmd::Base::Add>(&command);
		REQUIRE(add != nullptr);
		REQUIRE(add->name);
		CHECK(add->name == "proj");
		CHECK_FALSE(add->path);
	}
	// projctl add <path> <name>
	SUBCASE("add with path and name") {
		const Command command = parse({"add", "/path/to/project", "proj"});
		const auto*   add     = std::get_if<Cmd::Base::Add>(&command);
		REQUIRE(add != nullptr);
		REQUIRE(add->name);
		CHECK(add->name == "proj");
		REQUIRE(add->path);
		CHECK(add->path == "/path/to/project");
	}
}

// projctl remove | rm <name>
TEST_CASE("remove") {
	// projctl rm <name>
	SUBCASE("rm alias") {
		const Command command = parse({"rm", "proj"});
		const auto*   remove  = std::get_if<Cmd::Base::Remove>(&command);
		REQUIRE(remove != nullptr);
		CHECK(remove->name == "proj");
	}
	// projctl remove <name>
	SUBCASE("plain remove") {
		const Command command = parse({"remove", "proj"});
		const auto*   remove  = std::get_if<Cmd::Base::Remove>(&command);
		REQUIRE(remove != nullptr);
		CHECK(remove->name == "proj");
	}
}

// projctl path [<name>]
TEST_CASE("path") {
	// projctl path
	SUBCASE("path with no name") {
		const Command command = parse({"path"});
		const auto*   path    = std::get_if<Cmd::Base::Path>(&command);
		REQUIRE(path != nullptr);
		CHECK_FALSE(path->name);
	}
	// projctl path <name>
	SUBCASE("path with name") {
		const Command command = parse({"path", "proj"});
		const auto*   path    = std::get_if<Cmd::Base::Path>(&command);
		REQUIRE(path != nullptr);
		REQUIRE(path->name);
		CHECK(path->name == "proj");
	}
}

// projctl open [<name>]
TEST_CASE("open") {
	// projctl open
	SUBCASE("open with no name") {
		const Command command = parse({"open"});
		const auto*   open    = std::get_if<Cmd::Base::Open>(&command);
		REQUIRE(open != nullptr);
		CHECK_FALSE(open->name);
	}
	// projctl open <name>
	SUBCASE("open with name") {
		const Command command = parse({"open", "proj"});
		const auto*   open    = std::get_if<Cmd::Base::Open>(&command);
		REQUIRE(open != nullptr);
		REQUIRE(open->name);
		CHECK(open->name == "proj");
	}
}

// projctl build [<name>]
TEST_CASE("build") {
	// projctl build
	SUBCASE("build with no name") {
		const Command command = parse({"build"});
		const auto*   build   = std::get_if<Cmd::Base::Build>(&command);
		REQUIRE(build != nullptr);
		CHECK_FALSE(build->name);
	}
	// projctl build <name>
	SUBCASE("build with name") {
		const Command command = parse({"build", "proj"});
		const auto*   build   = std::get_if<Cmd::Base::Build>(&command);
		REQUIRE(build != nullptr);
		REQUIRE(build->name);
		CHECK(build->name == "proj");
	}
}

// projctl run [<name>]
TEST_CASE("run") {
	// projctl run
	SUBCASE("run with no name") {
		const Command command = parse({"run"});
		const auto*   run     = std::get_if<Cmd::Base::Run>(&command);
		REQUIRE(run != nullptr);
		CHECK_FALSE(run->name);
	}
	// projctl run <name>
	SUBCASE("run with name") {
		const Command command = parse({"run", "proj"});
		const auto*   run     = std::get_if<Cmd::Base::Run>(&command);
		REQUIRE(run != nullptr);
		REQUIRE(run->name);
		CHECK(run->name == "proj");
	}
}

// projctl branch [[<branch>] <name>]
TEST_CASE("branch") {
	// projctl branch
	SUBCASE("branch with no name") {
		const Command command = parse({"branch"});
		const auto*   branch  = std::get_if<Cmd::Git::Branch>(&command);
		REQUIRE(branch != nullptr);
		CHECK_FALSE(branch->name);
		CHECK_FALSE(branch->branch_name);
	}
	// projctl branch <name>
	SUBCASE("branch with name") {
		const Command command = parse({"branch", "proj"});
		const auto*   branch  = std::get_if<Cmd::Git::Branch>(&command);
		REQUIRE(branch != nullptr);
		REQUIRE(branch->name);
		CHECK(branch->name == "proj");
		CHECK_FALSE(branch->branch_name);
	}
	// projctl branch <branch_name> <name>
	SUBCASE("branch with name and branch_name") {
		const Command command = parse({"branch", "feature", "proj"});
		const auto*   branch  = std::get_if<Cmd::Git::Branch>(&command);
		REQUIRE(branch != nullptr);
		CHECK(branch->name == "proj");
		REQUIRE(branch->branch_name);
		CHECK(*branch->branch_name == "feature");
	}
}

// projctl status [<name> | --all | -a]
TEST_CASE("status") {
	// projctl status
	SUBCASE("status with no name") {
		const Command command = parse({"status"});
		const auto*   status  = std::get_if<Cmd::Git::Status>(&command);
		REQUIRE(status != nullptr);
		CHECK_FALSE(status->name);
		CHECK_FALSE(status->all);
	}
	// projctl status <name>
	SUBCASE("status with name") {
		const Command command = parse({"status", "proj"});
		const auto*   status  = std::get_if<Cmd::Git::Status>(&command);
		REQUIRE(status != nullptr);
		REQUIRE(status->name);
		CHECK(status->name == "proj");
		CHECK_FALSE(status->all);
	}
	// projctl status --all
	SUBCASE("status --all") {
		const Command command = parse({"status", "--all"});
		const auto*   status  = std::get_if<Cmd::Git::Status>(&command);
		REQUIRE(status != nullptr);
		CHECK_FALSE(status->name);
		CHECK(status->all);
	}
	// projctl status -a
	SUBCASE("status -a") {
		const Command command = parse({"status", "-a"});
		const auto*   status  = std::get_if<Cmd::Git::Status>(&command);
		REQUIRE(status != nullptr);
		CHECK_FALSE(status->name);
		CHECK(status->all);
	}
}

// projctl fetch [<name> | --all | -a]
TEST_CASE("fetch") {
	// projctl fetch
	SUBCASE("fetch with no name") {
		const Command command = parse({"fetch"});
		const auto*   fetch   = std::get_if<Cmd::Git::Fetch>(&command);
		REQUIRE(fetch != nullptr);
		CHECK_FALSE(fetch->name);
		CHECK_FALSE(fetch->all);
	}
	// projctl fetch <name>
	SUBCASE("fetch with name") {
		const Command command = parse({"fetch", "proj"});
		const auto*   fetch   = std::get_if<Cmd::Git::Fetch>(&command);
		REQUIRE(fetch != nullptr);
		REQUIRE(fetch->name);
		CHECK(fetch->name == "proj");
		CHECK_FALSE(fetch->all);
	}
	// projctl fetch --all
	SUBCASE("fetch --all") {
		const Command command = parse({"fetch", "--all"});
		const auto*   fetch   = std::get_if<Cmd::Git::Fetch>(&command);
		REQUIRE(fetch != nullptr);
		CHECK_FALSE(fetch->name);
		CHECK(fetch->all);
	}
	// projctl fetch -a
	SUBCASE("fetch -a") {
		const Command command = parse({"fetch", "-a"});
		const auto*   fetch   = std::get_if<Cmd::Git::Fetch>(&command);
		REQUIRE(fetch != nullptr);
		CHECK_FALSE(fetch->name);
		CHECK(fetch->all);
	}
}

// projctl pull [<name> | --all | -a]
TEST_CASE("pull") {
	// projctl pull
	SUBCASE("pull with no name") {
		const Command command = parse({"pull"});
		const auto*   pull    = std::get_if<Cmd::Git::Pull>(&command);
		REQUIRE(pull != nullptr);
		CHECK_FALSE(pull->name);
		CHECK_FALSE(pull->all);
	}
	// projctl pull <name>
	SUBCASE("pull with name") {
		const Command command = parse({"pull", "proj"});
		const auto*   pull    = std::get_if<Cmd::Git::Pull>(&command);
		REQUIRE(pull != nullptr);
		REQUIRE(pull->name);
		CHECK(pull->name == "proj");
		CHECK_FALSE(pull->all);
	}
	// projctl pull --all
	SUBCASE("pull --all") {
		const Command command = parse({"pull", "--all"});
		const auto*   pull    = std::get_if<Cmd::Git::Pull>(&command);
		REQUIRE(pull != nullptr);
		CHECK_FALSE(pull->name);
		CHECK(pull->all);
	}
	// projctl pull -a
	SUBCASE("pull -a") {
		const Command command = parse({"pull", "-a"});
		const auto*   pull    = std::get_if<Cmd::Git::Pull>(&command);
		REQUIRE(pull != nullptr);
		CHECK_FALSE(pull->name);
		CHECK(pull->all);
	}
}

// projctl merge [<name>]
TEST_CASE("merge") {
	// projctl merge
	SUBCASE("merge with no name") {
		const Command command = parse({"merge"});
		const auto*   merge   = std::get_if<Cmd::Git::Merge>(&command);
		REQUIRE(merge != nullptr);
		CHECK_FALSE(merge->name);
	}
	// projctl merge <name>
	SUBCASE("merge with name") {
		const Command command = parse({"merge", "proj"});
		const auto*   merge   = std::get_if<Cmd::Git::Merge>(&command);
		REQUIRE(merge != nullptr);
		REQUIRE(merge->name);
		CHECK(merge->name == "proj");
	}
}

// projctl commit [[<message>] <name>]
TEST_CASE("commit") {
	// projctl commit
	SUBCASE("commit with no name") {
		const Command command = parse({"commit"});
		const auto*   commit  = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		CHECK_FALSE(commit->name);
		CHECK_FALSE(commit->message);
	}
	// projctl commit <name>
	SUBCASE("commit with name") {
		const Command command = parse({"commit", "proj"});
		const auto*   commit  = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		REQUIRE(commit->name);
		CHECK(commit->name == "proj");
		CHECK_FALSE(commit->message);
	}
	// projctl commit <commit_message> <name>
	SUBCASE("commit with message") {
		const Command command = parse({"commit", "Fix bug", "proj"});
		const auto*   commit  = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		REQUIRE(commit->name);
		CHECK(commit->name == "proj");
		REQUIRE(commit->message);
		CHECK(commit->message == "Fix bug");
	}
	// projctl commit -u <message> <name>
	SUBCASE("commit -u with message and name") {
		const Command command = parse({"commit", "-u", "fix typo", "proj"});
		const auto*   commit  = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		CHECK(commit->tracked_only);
		CHECK(commit->message == "fix typo");
		CHECK(commit->name == "proj");
	}
	// projctl commit -u
	SUBCASE("commit -u alone") {
		const Command command = parse({"commit", "-u"});
		const auto*   commit  = std::get_if<Cmd::Git::Commit>(&command);
		REQUIRE(commit != nullptr);
		CHECK(commit->tracked_only);
		CHECK_FALSE(commit->name);
		CHECK_FALSE(commit->message);
	}
}

// projctl push [-u] [<name>]
TEST_CASE("push") {
	// projctl push
	SUBCASE("push with no name") {
		const Command command = parse({"push"});
		const auto*   push    = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		CHECK_FALSE(push->name);
		CHECK_FALSE(push->branch_upstream);
	}
	// projctl push <name>
	SUBCASE("plain push") {
		const Command command = parse({"push", "proj"});
		const auto*   push    = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		REQUIRE(push->name);
		CHECK(push->name == "proj");
		CHECK_FALSE(push->branch_upstream);
	}
	// projctl push -u
	SUBCASE("push upstream with no name") {
		const Command command = parse({"push", "-u"});
		const auto*   push    = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		CHECK_FALSE(push->name);
		CHECK(push->branch_upstream);
	}
	// projctl push -u <name>
	SUBCASE("push upstream") {
		const Command command = parse({"push", "-u", "proj"});
		const auto*   push    = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		REQUIRE(push->name);
		CHECK(push->name == "proj");
		CHECK(push->branch_upstream);
	}
}

/*
For later to implement (git pipeling):
projctl pull merge commit push [name]
projctl pull merge commit [commit_msg] push [name]
projctl pull merge commit push -u [name]
projctl pull merge commit [commit_message] push -u [name]
*/

// projctl git | repos
TEST_CASE("gh commands") {
	// projctl git
	SUBCASE("gh status") {
		const Command command   = parse({"git"});
		const auto*   gh_status = std::get_if<Cmd::GH::Status>(&command);
		REQUIRE(gh_status != nullptr);
	}
	// projctl repos
	SUBCASE("gh repos") {
		const Command command  = parse({"repos"});
		const auto*   gh_repos = std::get_if<Cmd::GH::RepoList>(&command);
		REQUIRE(gh_repos != nullptr);
	}
}

// projctl ilist [<name> | --all | -a]
// projctl icreate [<name>]
TEST_CASE("gh issue commands") {
	// projctl ilist
	SUBCASE("gh issue list with no name") {
		const Command command       = parse({"ilist"});
		const auto*   gh_issue_list = std::get_if<Cmd::GH::IssueList>(&command);
		REQUIRE(gh_issue_list != nullptr);
		CHECK_FALSE(gh_issue_list->name);
		CHECK_FALSE(gh_issue_list->all);
	}
	// projctl ilist <name>
	SUBCASE("gh issue list with name") {
		const Command command       = parse({"ilist", "proj"});
		const auto*   gh_issue_list = std::get_if<Cmd::GH::IssueList>(&command);
		REQUIRE(gh_issue_list != nullptr);
		REQUIRE(gh_issue_list->name);
		CHECK(gh_issue_list->name == "proj");
		CHECK_FALSE(gh_issue_list->all);
	}
	// projctl ilist --all
	SUBCASE("gh issue list --all") {
		const Command command       = parse({"ilist", "--all"});
		const auto*   gh_issue_list = std::get_if<Cmd::GH::IssueList>(&command);
		REQUIRE(gh_issue_list != nullptr);
		CHECK_FALSE(gh_issue_list->name);
		CHECK(gh_issue_list->all);
	}
	// projctl ilist -a
	SUBCASE("gh issue list -a") {
		const Command command       = parse({"ilist", "-a"});
		const auto*   gh_issue_list = std::get_if<Cmd::GH::IssueList>(&command);
		REQUIRE(gh_issue_list != nullptr);
		CHECK_FALSE(gh_issue_list->name);
		CHECK(gh_issue_list->all);
	}
	// projctl icreate
	SUBCASE("gh issue create with no name") {
		const Command command         = parse({"icreate"});
		const auto*   gh_issue_create = std::get_if<Cmd::GH::IssueCreate>(&command);
		REQUIRE(gh_issue_create != nullptr);
		CHECK_FALSE(gh_issue_create->name);
	}
	// projctl icreate <name>
	SUBCASE("gh issue create with name") {
		const Command command         = parse({"icreate", "proj"});
		const auto*   gh_issue_create = std::get_if<Cmd::GH::IssueCreate>(&command);
		REQUIRE(gh_issue_create != nullptr);
		REQUIRE(gh_issue_create->name);
		CHECK(gh_issue_create->name == "proj");
	}
}

// projctl reqlist [<name> | --all | -a]
// projctl reqcreate [<name>]
// projctl reqmerge [<name>]
TEST_CASE("gh request commands") {
	// projctl reqlist
	SUBCASE("gh request list with no name") {
		const Command command         = parse({"reqlist"});
		const auto*   gh_request_list = std::get_if<Cmd::GH::RequestList>(&command);
		REQUIRE(gh_request_list != nullptr);
		CHECK_FALSE(gh_request_list->name);
		CHECK_FALSE(gh_request_list->all);
	}
	// projctl reqlist <name>
	SUBCASE("gh request list with name") {
		const Command command         = parse({"reqlist", "proj"});
		const auto*   gh_request_list = std::get_if<Cmd::GH::RequestList>(&command);
		REQUIRE(gh_request_list != nullptr);
		REQUIRE(gh_request_list->name);
		CHECK(gh_request_list->name == "proj");
		CHECK_FALSE(gh_request_list->all);
	}
	// projctl reqlist --all
	SUBCASE("gh request list --all") {
		const Command command         = parse({"reqlist", "--all"});
		const auto*   gh_request_list = std::get_if<Cmd::GH::RequestList>(&command);
		REQUIRE(gh_request_list != nullptr);
		CHECK_FALSE(gh_request_list->name);
		CHECK(gh_request_list->all);
	}
	// projctl reqlist -a
	SUBCASE("gh request list -a") {
		const Command command         = parse({"reqlist", "-a"});
		const auto*   gh_request_list = std::get_if<Cmd::GH::RequestList>(&command);
		REQUIRE(gh_request_list != nullptr);
		CHECK_FALSE(gh_request_list->name);
		CHECK(gh_request_list->all);
	}
	// projctl reqcreate
	SUBCASE("gh request create with no name") {
		const Command command           = parse({"reqcreate"});
		const auto*   gh_request_create = std::get_if<Cmd::GH::RequestCreate>(&command);
		REQUIRE(gh_request_create != nullptr);
		CHECK_FALSE(gh_request_create->name);
	}
	// projctl reqcreate <name>
	SUBCASE("gh request create with name") {
		const Command command           = parse({"reqcreate", "proj"});
		const auto*   gh_request_create = std::get_if<Cmd::GH::RequestCreate>(&command);
		REQUIRE(gh_request_create != nullptr);
		REQUIRE(gh_request_create->name);
		CHECK(gh_request_create->name == "proj");
	}
	// projctl reqmerge
	SUBCASE("gh request merge with no name") {
		const Command command          = parse({"reqmerge"});
		const auto*   gh_request_merge = std::get_if<Cmd::GH::RequestMerge>(&command);
		REQUIRE(gh_request_merge != nullptr);
		CHECK_FALSE(gh_request_merge->name);
	}
	// projctl reqmerge <name>
	SUBCASE("gh request merge with name") {
		const Command command          = parse({"reqmerge", "proj"});
		const auto*   gh_request_merge = std::get_if<Cmd::GH::RequestMerge>(&command);
		REQUIRE(gh_request_merge != nullptr);
		REQUIRE(gh_request_merge->name);
		CHECK(gh_request_merge->name == "proj");
	}
}

namespace {
	bool rejected(std::vector<std::string_view> args) {
		return std::holds_alternative<Cmd::Unknown>(parser(args));
	}
} // namespace

TEST_CASE("bad input is rejected") {
	SUBCASE("no command / unknown command") {
		CHECK(rejected({}));
		CHECK(rejected({"nonsense"}));
		CHECK(rejected({"nonsense", "proj"}));
		CHECK(rejected({"Status", "proj"})); // commands are case-sensitive
		CHECK(rejected({"-a"}));
		CHECK(rejected({"--all"}));
	}

	SUBCASE("commands without arguments") {
		for (std::string_view command : {"git", "repos"}) {
			CAPTURE(command);
			CHECK(rejected({command, "proj"}));
			CHECK(rejected({command, "."}));
			CHECK(rejected({command, "-a"}));
		}
	}

	SUBCASE("list") {
		CHECK(rejected({"list", "proj"}));
		CHECK(rejected({"list", "."}));
		CHECK(rejected({"list", "gits", "proj"}));
		CHECK(rejected({"list", "gits", "gits"}));
		CHECK(rejected({"list", "--all"}));
		CHECK(rejected({"list", "-a"}));
	}

	SUBCASE("add") {
		CHECK(rejected({"add", "-a"})); // name can't be an option
		CHECK(rejected({"add", "--all"}));
		CHECK(rejected({"add", "-a", "proj"})); // path can't be an option
		CHECK(rejected({"add", "/some/path", "-a"}));
		CHECK(rejected({"add", "/some/path", "proj", "extra"}));
	}

	SUBCASE("remove / rm (name required)") {
		for (std::string_view command : {"remove", "rm"}) {
			CAPTURE(command);
			CHECK(rejected({command}));
			CHECK(rejected({command, "."})); // never "remove whatever I'm standing in"
			CHECK(rejected({command, "a", "b"}));
			CHECK(rejected({command, "--all"})); // never "remove everything"
			CHECK(rejected({command, "-a"}));
		}
	}

	SUBCASE("optional name: too many arguments / options") {
		for (std::string_view command : {"path", "open", "build", "run", "merge", "icreate", "reqcreate", "reqmerge"}) {
			CAPTURE(command);
			CHECK(rejected({command, "a", "b"}));
			CHECK(rejected({command, ".", "proj"}));
			CHECK(rejected({command, "--all"}));
			CHECK(rejected({command, "-a"}));
			CHECK(rejected({command, "-x"}));
		}
	}

	SUBCASE("name or --all / -a: not both, not more") {
		for (std::string_view command : {"status", "fetch", "pull", "ilist", "reqlist"}) {
			CAPTURE(command);
			CHECK(rejected({command, "a", "b"}));
			CHECK(rejected({command, "--all", "proj"}));
			CHECK(rejected({command, "proj", "--all"}));
			CHECK(rejected({command, "-a", "proj"}));
			CHECK(rejected({command, "--all", "-a"}));
			CHECK(rejected({command, "-a", "-a"}));
			CHECK(rejected({command, "-x"})); // only -a / --all are options here
			CHECK(rejected({command, "--al"}));
		}
	}

	SUBCASE("branch (name required)") {
		CHECK(rejected({"branch", "-a"}));
		CHECK(rejected({"branch", "-x", "proj"})); // branch name can't be an option
		CHECK(rejected({"branch", "feature", "-a"}));
		CHECK(rejected({"branch", "feature", "proj", "extra"}));
	}

	SUBCASE("commit") {
		CHECK(rejected({"commit", "-a"})); // name can't be an option
		CHECK(rejected({"commit", "message", "-a"}));
		CHECK(rejected({"commit", "message", "proj", "extra"}));
	}

	SUBCASE("push") {
		CHECK(rejected({"push", "a", "b"}));
		CHECK(rejected({"push", "proj", "-u"})); // -u only directly after push
		CHECK(rejected({"push", "-u", "-u"}));
		CHECK(rejected({"push", "-u", "-u", "proj"}));
		CHECK(rejected({"push", "-u", "a", "b"}));
		CHECK(rejected({"push", "-u", "-a"}));
		CHECK(rejected({"push", "-x"})); // unknown flag
		CHECK(rejected({"push", "-x", "proj"}));
		CHECK(rejected({"push", "--all"}));
		CHECK(rejected({"push", "-a"}));
	}
}
