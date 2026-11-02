#include "projctl.hpp"

#include <print>


int ProjCtl::handle(const Cmd::Unknown& cmd) const {
	std::println(
			"\n\n{}Unrecognized command{}\n\n",
			color_code(config.error_color),
			Style::RESET
			);
	return 1;
}


int ProjCtl::handle(const Cmd::Base::List& cmd) const {
	std::expected<std::string, std::string> result;

	if (cmd.gits_only) {
		result = list_gits();
	} else {
		result = list();
	}

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Base::Path& cmd) const {
	std::expected<std::string, std::string> result = path_show(cmd.name);

	if (!result) {
		std::println(
				"{}{}{}{}",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("{}", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Base::Add& cmd) {
	std::expected<std::string, std::string> result;

	if (cmd.name == ".") {
		result = add_current();
	} else {
		result = add(cmd.name, cmd.path);
	}

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Base::Remove& cmd) {
	std::expected<std::string, std::string> result = remove(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Base::Open& cmd) const {
	std::expected<std::string, std::string> result = open(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Base::Build& cmd) const {
	std::expected<int, std::string> result = build(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	return *result;
}


int ProjCtl::handle(const Cmd::Base::Run& cmd) const {
	std::expected<int, std::string> result = run(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	return *result;
}


int ProjCtl::handle(const Cmd::Git::Status& cmd) const {
	std::expected<std::string, std::string> result = status(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Git::Commit& cmd) const {
	std::expected<std::string, std::string> result = git_commit(cmd.name, cmd.message);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Git::Push& cmd) const {
	std::expected<std::string, std::string> result;

	if (cmd.branch_upstream) {
		result = git_push_branch(cmd.name);
	} else {
		result = git_push(cmd.name);
	}

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Git::Pull& cmd) const {
	std::expected<std::string, std::string> result = git_pull(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Git::Fetch& cmd) const {
	std::expected<std::string, std::string> result = git_fetch(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Git::Branch& cmd) const {
	std::expected<std::string, std::string> result;

	if (cmd.branch_name) {
		result = git_branch(cmd.name, *cmd.branch_name);
	} else {
		result = git_branch_list(cmd.name);
	}

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::Git::Merge& cmd) const {
	std::expected<std::string, std::string> result = git_merge(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::GH::Status& cmd) const {
	std::expected<std::string, std::string> result = g_status();

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::GH::RepoList& cmd) const {
	std::expected<std::string, std::string> result = repo_list();

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::GH::IssueList& cmd) const {
	std::expected<std::string, std::string> result = issue_list(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::GH::IssueCreate& cmd) const {
	std::expected<int, std::string> result = issue_create(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	return *result;
}


int ProjCtl::handle(const Cmd::GH::RequestList& cmd) const {
	std::expected<std::string, std::string> result = request_list(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	std::println("\n\n{}\n\n", *result);
	return 0;
}


int ProjCtl::handle(const Cmd::GH::RequestCreate& cmd) const {
	std::expected<int, std::string> result = request_create(cmd.name);

	if (!result) {
		std::println(
				"\n\n{}{}{}{}\n\n",
				color_code(config.error_color),
				Style::CROSS,
				result.error(),
				Style::RESET
				);
		return 1;
	}

	return *result;
}
