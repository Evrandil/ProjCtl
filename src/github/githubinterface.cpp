#include "githubinterface.hpp"

#include "clistate.hpp"
#include "systeminterface.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <expected>
#include <format>
#include <ranges>
#include <string>
#include <vector>

auto gh_command(std::vector<std::string> args) -> std::vector<std::string> {
	std::vector<std::string> command =
		std::vector<std::string>{"env", "NO_COLOR=1", "CLICOLOR_FORCE=0", "GH_NO_UPDATE_NOTIFIER=1"};
	command.append_range(args | std::views::as_rvalue);
	return command;
}

auto GitHubInterface::state() -> CliState {
	std::expected result = SystemInterface::run(gh_command(std::vector<std::string>{"gh", "auth", "status"}));

	if (!result) {
		return CliState::NotInstalled;
	}

	return result->exit_code == 0 ? CliState::Ready : CliState::NotAuthenticated;
}

auto GitHubInterface::repo_list() -> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(
		gh_command(std::vector<std::string>{"gh", "repo", "list", "--json", "nameWithOwner", "--limit", "100"})
	);

	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	std::string output;

	const nlohmann::json json = nlohmann::json::parse(result->text, nullptr, false);
	if (json.is_discarded()) {
		return std::unexpected(std::format("Unexpected output from gh:\n{}", result->text));
	}

	for (const nlohmann::json& repo : json) {
		output += repo.at("nameWithOwner").get<std::string>();
		output += '\n';
	}

	return output;
}
