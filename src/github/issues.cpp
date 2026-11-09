#include "githubinterface.hpp"

#include "row.hpp"
#include "systeminterface.hpp"

#include <expected>
#include <filesystem>
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <utility>
#include <vector>

auto GitHubInterface::issue_list(const std::filesystem::path& path) -> std::expected<std::vector<Row>, std::string> {
	const std::expected result = SystemInterface::run(
		gh_command(
			std::vector<std::string>{
				"gh",
				"issue",
				"list",
				"--json",
				"number,title,state,author,labels,createdAt,updatedAt,parent",
			}
		),
		path
	);

	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	const nlohmann::json json = nlohmann::json::parse(result->text, nullptr, false);
	if (json.is_discarded()) {
		return std::unexpected(std::format("Unexpected output from gh:\n{}", result->text));
	}

	std::vector<Row> issues;
	issues.reserve(json.size());

	for (const nlohmann::json& json_issue : json) {
		Row issue;
		issue.number  = json_issue.at("number").get<int>();
		issue.title   = json_issue.at("title").get<std::string>();
		issue.state   = json_issue.at("state").get<std::string>();
		issue.created = json_issue.at("createdAt").get<std::string>();
		issue.updated = json_issue.at("updatedAt").get<std::string>();
		if (!json_issue.at("parent").is_null()) {
			issue.parent = json_issue.at("parent").at("title").get<std::string>();
		}

		if (!json_issue.at("author").is_null()) {
			issue.author = json_issue.at("author").at("login").get<std::string>();
		}

		for (const nlohmann::json& label : json_issue.at("labels")) {
			issue.labels.push_back(label.at("name").get<std::string>());
		}

		issues.push_back(std::move(issue));
	}

	return issues;
}

auto GitHubInterface::issue_create(const std::filesystem::path& path) -> std::expected<int, std::string> {
	return SystemInterface::run_interactive(std::vector<std::string>{"gh", "issue", "create"}, path);
}

auto GitHubInterface::issue_edit(const std::filesystem::path& path, int issue_number)
	-> std::expected<int, std::string> {
	return SystemInterface::run_interactive(
		std::vector<std::string>{"gh", "issue", "edit", std::to_string(issue_number)},
		path
	);
}

auto GitHubInterface::issue_view(const std::filesystem::path& path, int issue_number)
	-> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(
		gh_command(std::vector<std::string>{"gh", "issue", "view", std::to_string(issue_number)}),
		path
	);

	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}

auto GitHubInterface::issue_close(const std::filesystem::path& path, int issue_number)
	-> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(
		gh_command(std::vector<std::string>{"gh", "issue", "close", std::to_string(issue_number)}),
		path
	);

	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}
