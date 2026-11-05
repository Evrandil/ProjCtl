#include "githubinterface.hpp"

#include "systeminterface.hpp"
#include <nlohmann/json.hpp>


/*
namespace
{
	std::vector<std::string> colors_helper(std::vector<std::string> args)
	{
		return std::vector<std::string>{"env", "NO_COLOR=1", "CLICOLOR_FORCE=0", "GH_NO_UPDATE_NOTIFIER=1"};
	}
}
*/


CliState GitHubInterface::state()
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"gh", "auth", "status"});

	if (!result)
	{
		return CliState::NotInstalled;
	}

	return result->exit_code == 0 ? CliState::Ready : CliState::NotAuthenticated;
}


std::expected<std::string, std::string> GitHubInterface::repo_list()
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"gh", "repo", "list", "--json", "nameWithOwner", "--jq", ".[].nameWithOwner", "--limit", "100"});

	if (!result)
	{
		return std::unexpected(result.error());
	}
	if (result->exit_code != 0)
	{
		return std::unexpected(result->text);
	}

	std::string output;
	for (const nlohmann::json& repo : nlohmann::json::parse(result->text))
	{
		output += repo.get<std::string>();
		output += '\n';
	}
	if (!output.empty())
	{
		output.pop_back();
	}

	return output;
}


std::expected<std::vector<Issue>, std::string> GitHubInterface::issue_list(std::string_view path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"env", "NO_COLOR=1", "CLICOLOR_FORCE=0", "GH_NO_UPDATE_NOTIFIER=1", "gh", "issue", "list", "--json", "number,title,state,author,labels,createdAt,updatedAt,parent"}, path);

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

	std::vector<Issue> issues;
	issues.reserve(json.size());

	for (const nlohmann::json& json_issue : json)
	{
		Issue issue;
		issue.number  = json_issue.at("number").get<int>();
		issue.title   = json_issue.at("title").get<std::string>();
		issue.state   = json_issue.at("state").get<std::string>();
		issue.created = json_issue.at("createdAt").get<std::string>();
		issue.updated = json_issue.at("updatedAt").get<std::string>();
		if (!json_issue.at("parent").is_null())
		{
			issue.parent = json_issue.at("parent").at("title").get<std::string>();
		}

		if (!json_issue.at("author").is_null())
		{
			issue.author = json_issue.at("author").at("login").get<std::string>();
		}

		for (const nlohmann::json& label : json_issue.at("labels"))
		{
			issue.labels.push_back(label.at("name").get<std::string>());
		}

		issues.push_back(std::move(issue));
	}

	return issues;
}


std::expected<int, std::string> GitHubInterface::issue_create(std::string_view path)
{
	return SystemInterface::run_interactive(std::vector<std::string>{"gh", "issue", "create"}, path);
}


std::expected<std::vector<Request>, std::string> GitHubInterface::request_list(std::string_view path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"env", "NO_COLOR=1", "CLICOLOR_FORCE=0", "GH_NO_UPDATE_NOTIFIER=1", "gh", "pr", "list", "--json", "number,title,state,author,labels,createdAt,updatedAt"}, path);

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


std::expected<int, std::string> GitHubInterface::request_create(std::string_view path)
{
	return SystemInterface::run_interactive(std::vector<std::string>{"gh", "pr", "create"}, path);
}


std::expected<std::string, std::string> GitHubInterface::request_merge(std::string_view path)
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
