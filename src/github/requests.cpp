#include "githubinterface.hpp"

#include "systeminterface.hpp"
#include <nlohmann/json.hpp>


std::expected<std::vector<Request>, std::string> GitHubInterface::request_list(const std::filesystem::path& path)
{
	std::expected result = SystemInterface::run(
		gh_command(std::vector<std::string>{"gh", "pr", "list", "--json", "number,title,state,author,labels,createdAt,updatedAt"}), path);

	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	nlohmann::json json = nlohmann::json::parse(result->text, nullptr, false);
	if (json.is_discarded())
	{
		return std::unexpected(std::format("Unexpected output from gh:\n{}", result->text));
	}

	std::vector<Request> requests;
	requests.reserve(json.size());

	for (const nlohmann::json& json_request : json)
	{
		Request request;
		request.number  = json_request.at("number").get<int>();
		request.title   = json_request.at("title").get<std::string>();
		request.state   = json_request.at("state").get<std::string>();
		request.created = json_request.at("createdAt").get<std::string>();
		request.updated = json_request.at("updatedAt").get<std::string>();

		if (!json_request.at("author").is_null())
		{
			request.author = json_request.at("author").at("login").get<std::string>();
		}

		for (const nlohmann::json& label : json_request.at("labels"))
		{
			request.labels.push_back(label.at("name").get<std::string>());
		}

		requests.push_back(std::move(request));
	}

	return requests;
}


std::expected<int, std::string> GitHubInterface::request_create(const std::filesystem::path& path)
{
	return SystemInterface::run_interactive(std::vector<std::string>{"gh", "pr", "create"}, path);
}

std::expected<int, std::string> GitHubInterface::request_edit(const std::filesystem::path& path, int request_number)
{
	return SystemInterface::run_interactive(std::vector<std::string>{"gh", "pr", "edit", std::to_string(request_number)}, path);
}

std::expected<std::string, std::string> GitHubInterface::request_close(const std::filesystem::path& path, int request_number)
{
	std::expected result = SystemInterface::run(gh_command(std::vector<std::string>{"gh", "pr", "close", std::to_string(request_number)}), path);

	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	return result->text;
}


std::expected<std::string, std::string> GitHubInterface::request_merge(const std::filesystem::path& path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"gh", "pr", "merge", "--squash", "--delete-branch"}, path);
	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	return result->text;
}
