#include "gitlabinterface.hpp"

#include "systeminterface.hpp"

#include <nlohmann/json.hpp>

auto GitLabInterface::state() -> CliState {
	std::expected result = SystemInterface::run(std::vector<std::string>{"glab", "auth", "status"});

	if (!result) {
		return CliState::NotInstalled;
	}

	return result->exit_code == 0 ? CliState::Ready : CliState::NotAuthenticated;
}

auto GitLabInterface::repo_list() -> std::expected<std::string, std::string> {
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "repo", "list", "--output", "json", "--per-page", "100"});

	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	std::string output;

	const nlohmann::json json = nlohmann::json::parse(result->text, nullptr, false);
	if (json.is_discarded()) {
		return std::unexpected(std::format("Unexpected output from glab:\n{}", result->text));
	}

	for (const nlohmann::json& repo : json) {
		output += repo.at("path_with_namespace").get<std::string>();
		output += '\n';
	}

	return output;
}
