#include <doctest/doctest.h>

#include "remote_url.hpp"


TEST_CASE("host_from_remote")
{
	SUBCASE("ssh, scp-like")
	{
		CHECK(host_from_remote("git@github.com:Evrandil/projctl.git") == HostOption::GitHub);
		CHECK(host_from_remote("git@gitlab.com:group/repo.git") == HostOption::GitLab);
		CHECK(host_from_remote("git@github.com:Evrandil/projctl") == HostOption::GitHub);
	}

	SUBCASE("ssh, URL form")
	{
		CHECK(host_from_remote("ssh://git@github.com/Evrandil/projctl.git") == HostOption::GitHub);
		CHECK(host_from_remote("ssh://git@gitlab.com:2222/group/repo.git") == HostOption::GitLab);   // custom port
	}

	SUBCASE("https / http")
	{
		CHECK(host_from_remote("https://github.com/Evrandil/projctl") == HostOption::GitHub);
		CHECK(host_from_remote("https://github.com/Evrandil/projctl.git") == HostOption::GitHub);
		CHECK(host_from_remote("https://github.com/Evrandil/projctl/") == HostOption::GitHub);
		CHECK(host_from_remote("https://user@gitlab.com/group/repo.git") == HostOption::GitLab);   // user in URL
		CHECK(host_from_remote("http://gitlab.com/group/repo") == HostOption::GitLab);
	}

	SUBCASE("self-hosted")
	{
		CHECK(host_from_remote("git@gitlab.company.com:team/repo.git") == HostOption::GitLab);
		CHECK(host_from_remote("https://gitlab.company.com/team/repo") == HostOption::GitLab);
	}

	SUBCASE("other hosts are Unknown, not errors")
	{
		CHECK(host_from_remote("git@codeberg.org:someone/repo.git") == HostOption::Unknown);
		CHECK(host_from_remote("https://bitbucket.org/someone/repo") == HostOption::Unknown);
	}

	SUBCASE("garbage is an error")
	{
		CHECK_FALSE(host_from_remote(""));
		CHECK_FALSE(host_from_remote("not a url"));
		CHECK_FALSE(host_from_remote("https://"));
	}
}


TEST_CASE("short_from_remote")
{
	SUBCASE("ssh and https give the same owner/repo")
	{
		CHECK(short_from_remote("git@github.com:Evrandil/projctl.git") == "Evrandil/projctl");
		CHECK(short_from_remote("ssh://git@github.com/Evrandil/projctl.git") == "Evrandil/projctl");
		CHECK(short_from_remote("https://github.com/Evrandil/projctl") == "Evrandil/projctl");
		CHECK(short_from_remote("https://github.com/Evrandil/projctl.git") == "Evrandil/projctl");
		CHECK(short_from_remote("https://github.com/Evrandil/projctl/") == "Evrandil/projctl");
		CHECK(short_from_remote("https://user@github.com/Evrandil/projctl") == "Evrandil/projctl");
	}

	SUBCASE("GitLab subgroups keep the full path")
	{
		CHECK(short_from_remote("git@gitlab.com:group/subgroup/repo.git") == "group/subgroup/repo");
		CHECK(short_from_remote("https://gitlab.com/group/subgroup/repo") == "group/subgroup/repo");
		CHECK(short_from_remote("ssh://git@gitlab.com:2222/group/subgroup/repo.git") == "group/subgroup/repo");
	}

	SUBCASE("garbage is an error")
	{
		CHECK_FALSE(short_from_remote(""));
		CHECK_FALSE(short_from_remote("not a url"));
		CHECK_FALSE(short_from_remote("https://github.com/"));
		CHECK_FALSE(short_from_remote("git@github.com:onlyrepo"));
	}
}
