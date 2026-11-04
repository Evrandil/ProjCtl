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


/*
projctl list 
projctl list gits

projctl status [name]
projctl status --all

projctl add [path] [name] 
projctl add . [name]

projctl remove [name]
projctl rm [name]

projctl open [name]

projctl path [name]

projctl build [name]

projctl run [name]

projctl branch [branch_name] [name]
projctl branch [name]

projctl fetch [name]
projctl fetch --all

projctl pull [name]
projctl pull --all

projctl commit [name]
projctl commit [commit_message] [name]

projctl push [name]
projctl push branch [name]

projctl automerge [name]

projctl pull automerge commit push [name]
projctl pull automerge commit [commit_msg] push [name]
projctl pull automerge commit push branch [name]
projctl pull automerge commit [commit_message] push branch [name]

projctl git

prjctl repos

projctl ilist [name]
projctl icreate [name]

projctl reqlist [name]
projctl reqcreate [name]
*/


TEST_CASE("push")
{
	SUBCASE("plain push")
	{
		const Command command = parse({"push", "proj"});
		const auto* push = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		CHECK(push->name == "proj");
		CHECK_FALSE(push->branch_upstream);
	}

	SUBCASE("push branch")
	{
		const Command command = parse({"push", "branch", "proj"});
		const auto* push = std::get_if<Cmd::Git::Push>(&command);
		REQUIRE(push != nullptr);
		CHECK(push->branch_upstream);
	}
}


TEST_CASE("bad input is rejected")
{
	CHECK(std::holds_alternative<Cmd::Unknown>(parse({})));
	CHECK(std::holds_alternative<Cmd::Unknown>(parse({"remove", "a", "b"})));
	CHECK(std::holds_alternative<Cmd::Unknown>(parse({"nonsense", "proj"})));
}
