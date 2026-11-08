#include "githubinterface.hpp"

#include "request.hpp"
#include "systeminterface.hpp"

#include <expected>
#include <filesystem>
#include <format>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <utility>
#include <vector>

auto GitHubInterface::request_list(const std::filesystem::path& path)
	-> std::expected<std::vector<Request>, std::string> {
	const std::expected result = SystemInterface::run(
		gh_command(
			std::vector<
				std::string>{"gh", "pr", "list", "--json", "number,title,state,author,labels,createdAt,updatedAt"}
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

	std::vector<Request> requests;
	requests.reserve(json.size());

	for (const nlohmann::json& json_request : json) {
		Request request;
		request.number  = json_request.at("number").get<int>();
		request.title   = json_request.at("title").get<std::string>();
		request.state   = json_request.at("state").get<std::string>();
		request.created = json_request.at("createdAt").get<std::string>();
		request.updated = json_request.at("updatedAt").get<std::string>();

		if (!json_request.at("author").is_null()) {
			request.author = json_request.at("author").at("login").get<std::string>();
		}

		for (const nlohmann::json& label : json_request.at("labels")) {
			request.labels.push_back(label.at("name").get<std::string>());
		}

		requests.push_back(std::move(request));
	}

	return requests;
}

auto GitHubInterface::request_create(const std::filesystem::path& path) -> std::expected<int, std::string> {
	return SystemInterface::run_interactive(std::vector<std::string>{"gh", "pr", "create"}, path);
}

auto GitHubInterface::request_edit(const std::filesystem::path& path, int request_number)
	-> std::expected<int, std::string> {
	return SystemInterface::run_interactive(
		std::vector<std::string>{"gh", "pr", "edit", std::to_string(request_number)},
		path
	);
}

auto GitHubInterface::request_view(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(
		gh_command(std::vector<std::string>{"gh", "pr", "close", std::to_string(request_number)}),
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

auto GitHubInterface::request_close(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(
		gh_command(std::vector<std::string>{"gh", "pr", "close", std::to_string(request_number)}),
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

auto GitHubInterface::request_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"gh", "pr", "merge", "--squash", "--delete-branch"}, path);
	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}
