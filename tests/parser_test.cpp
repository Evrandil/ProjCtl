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
