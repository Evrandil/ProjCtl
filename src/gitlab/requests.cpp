#include "gitlabinterface.hpp"

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

auto GitLabInterface::request_list(const std::filesystem::path& path)
	-> std::expected<std::vector<Request>, std::string> {
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "mr", "list", "--output", "json"}, path);

	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	const nlohmann::json json = nlohmann::json::parse(result->text, nullptr, false);
	if (json.is_discarded()) {
		return std::unexpected(std::format("Unexpected output from glab:\n{}", result->text));
	}

	std::vector<Request> requests;
	requests.reserve(json.size());

	for (const nlohmann::json& json_request : json) {
		Request request;
		request.number  = json_request.at("iid").get<int>();
		request.title   = json_request.at("title").get<std::string>();
		request.state   = json_request.at("state").get<std::string>();
		request.created = json_request.at("created_at").get<std::string>();
		request.updated = json_request.at("updated_at").get<std::string>();
		request.author  = json_request.at("author").at("username").get<std::string>();

		for (const nlohmann::json& label : json_request.at("labels")) {
			request.labels.push_back(label.get<std::string>());
		}

		requests.push_back(std::move(request));
	}

	return requests;
}

auto GitLabInterface::request_create(const std::filesystem::path& path) -> std::expected<int, std::string> {
	return SystemInterface::run_interactive(std::vector<std::string>{"glab", "mr", "create"}, path);
}

auto GitLabInterface::request_edit(const std::filesystem::path& path, int request_number)
	-> std::expected<int, std::string> {
	return SystemInterface::run_interactive(
		std::vector<std::string>{"glab", "mr", "edit", std::to_string(request_number)},
		path
	);
}

auto GitLabInterface::request_view(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "mr", "close", std::to_string(request_number)}, path);
	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}

auto GitLabInterface::request_close(const std::filesystem::path& path, int request_number)
	-> std::expected<std::string, std::string> {
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "mr", "close", std::to_string(request_number)}, path);
	if (!result) {
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0) {
		return std::unexpected(result->text);
	}

	return result->text;
}

auto GitLabInterface::request_merge(const std::filesystem::path& path) -> std::expected<std::string, std::string> {
	std::expected result = SystemInterface::run(
		std::vector<std::string>{"glab", "mr", "merge", "--squash", "--remove-source-branch"},
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
