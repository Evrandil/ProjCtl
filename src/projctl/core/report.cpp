#include "projctl.hpp"

#include <print>

int ProjCtl::report(const std::expected<std::string, std::string>& result) const
{
	if (!result)
	{
		std::println(stderr, "\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	std::println(stdout, "\n\n{}\n\n", *result);
	return 0;
}

int ProjCtl::report(const std::expected<int, std::string>& result) const
{
	if (!result)
	{
		std::println(stderr, "\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	return *result;
}

int ProjCtl::report(const BatchResult& results) const
{
	std::println(stdout, "\n\n{}\n\n", results.output);
	if (results.failed > 0)
	{
		std::println(stderr, "{}{}{}{} failed\n\n", color_code(config.error_color), Style::CROSS, results.failed, Style::RESET);
		return 1;
	}
	return 0;
}

int ProjCtl::report_save(const std::expected<std::string, std::string>& result) const
{
	if (!result)
	{
		std::println(stderr, "\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	std::expected saved = save();
	if (!saved)
	{
		std::println(stderr, "\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, saved.error(), Style::RESET);
		return 1;
	}

	std::println(stdout, "\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Unknown&) const
{
	std::println(stderr, "\n\n{}Unrecognized command{}\n\n", color_code(config.error_color), Style::RESET);
	return 1;
}


int ProjCtl::handle(const Cmd::Base::List& cmd) const
{
	if (cmd.gits_only)
	{
		return report(list_gits());
	}
	return report(list());
}


int ProjCtl::handle(const Cmd::Base::Path& cmd) const
{
	std::expected<std::string, std::string> result = path_show(cmd.name);

	if (!result)
	{
		std::println(stderr, "{}{}{}{}", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	std::println("{}", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Base::Add& cmd)
{
	if (cmd.name == ".")
	{
		return report_save(add_current());
	}
	return report_save(add(cmd.name, cmd.path));
}


int ProjCtl::handle(const Cmd::Base::Remove& cmd)
{
	return report_save(remove(cmd.name));
}




int ProjCtl::handle(const Cmd::GH::Status&) const
{
	return report(g_status());
}


int ProjCtl::handle(const Cmd::GH::RepoList&) const
{
	return report(repo_list());
}


int ProjCtl::handle(const Cmd::GH::IssueList& cmd) const
{
	return report(issue_list(cmd.name));
}


int ProjCtl::handle(const Cmd::GH::IssueCreate& cmd) const
{
	std::expected<int, std::string> result = issue_create(cmd.name);

	if (!result)
	{
		std::println("\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	return *result;
}


int ProjCtl::handle(const Cmd::GH::RequestList& cmd) const
{
	return report(request_list(cmd.name));
}


int ProjCtl::handle(const Cmd::GH::RequestCreate& cmd) const
{
	std::expected<int, std::string> result = request_create(cmd.name);

	if (!result)
	{
		std::println("\n\n{}{}{}{}\n\n", color_code(config.error_color), Style::CROSS, result.error(), Style::RESET);
		return 1;
	}

	return *result;
}


int ProjCtl::handle(const Cmd::GH::RequestMerge& cmd) const
{
	return report(request_merge(cmd.name));
}
