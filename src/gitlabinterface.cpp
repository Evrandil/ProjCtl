#include "gitlabinterface.hpp"

#include "systeminterface.hpp"
#include <nlohmann/json.hpp>


CliState GitLabInterface::state()
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"glab", "auth", "status"});

	if (!result)
	{
		return CliState::NotInstalled;
	}

	return result->exit_code == 0 ? CliState::Ready : CliState::NotAuthenticated;
}


std::expected<std::string, std::string> GitLabInterface::repo_list()
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"glab", "repo", "list", "--output", "json", "--per-page", "100"});

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
		output += repo.at("path_with_namespace").get<std::string>();
		output += '\n';
	}
	if (!output.empty())
	{
		output.pop_back();
	}

	return output;
}


std::expected<std::vector<Issue>, std::string> GitLabInterface::issue_list(std::string_view path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"glab", "issue", "list", "--output", "json"}, path);

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
		return std::unexpected(std::format("Unexpected output from glab:\n{}", result->text));
	}

	std::vector<Issue> issues;
	issues.reserve(json.size());

	for (const nlohmann::json& json_issue : json)
	{
		Issue issue;
		issue.number  = json_issue.at("iid").get<int>();
		issue.title   = json_issue.at("title").get<std::string>();
		issue.state   = json_issue.at("state").get<std::string>();
		issue.created = json_issue.at("created_at").get<std::string>();
		issue.updated = json_issue.at("updated_at").get<std::string>();
		issue.author  = json_issue.at("author").at("username").get<std::string>();

		for (const nlohmann::json& label : json_issue.at("labels"))
		{
			issue.labels.push_back(label.get<std::string>());
		}

		issues.push_back(std::move(issue));
	}

	return issues;
}


std::expected<int, std::string> GitLabInterface::issue_create(std::string_view path) { return SystemInterface::run_interactive(std::vector<std::string>{"glab", "issue", "create"}, path); }


std::expected<std::vector<Request>, std::string> GitLabInterface::request_list(std::string_view path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"glab", "mr", "list", "--output", "json"}, path);

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
		return std::unexpected(std::format("Unexpected output from glab:\n{}", result->text));
	}

	std::vector<Request> requests;
	requests.reserve(json.size());

	for (const nlohmann::json& json_request : json)
	{
		Request request;
		request.number  = json_request.at("iid").get<int>();
		request.title   = json_request.at("title").get<std::string>();
		request.state   = json_request.at("state").get<std::string>();
		request.created = json_request.at("created_at").get<std::string>();
		request.updated = json_request.at("updated_at").get<std::string>();
		request.author  = json_request.at("author").at("username").get<std::string>();

		for (const nlohmann::json& label : json_request.at("labels"))
		{
			request.labels.push_back(label.get<std::string>());
		}

		requests.push_back(std::move(request));
	}

	return requests;
}


std::expected<int, std::string> GitLabInterface::request_create(std::string_view path) { return SystemInterface::run_interactive(std::vector<std::string>{"glab", "mr", "create"}, path); }


std::expected<std::string, std::string> GitLabInterface::request_merge(std::string_view path)
{
	std::expected result = SystemInterface::run(std::vector<std::string>{"glab", "mr", "merge", "--squash", "--remove-source-branch"}, path);
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
