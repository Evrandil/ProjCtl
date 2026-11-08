#include "gitlabinterface.hpp"

#include "systeminterface.hpp"
#include <nlohmann/json.hpp>


std::expected<std::vector<Issue>, std::string> GitLabInterface::issue_list(const std::filesystem::path& path)
{
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "issue", "list", "--output", "json"}, path);

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


std::expected<int, std::string> GitLabInterface::issue_create(const std::filesystem::path& path)
{
	return SystemInterface::run_interactive(std::vector<std::string>{"glab", "issue", "create"}, path);
}

std::expected<int, std::string> GitLabInterface::issue_edit(const std::filesystem::path& path, int issue_number)
{
	return SystemInterface::run_interactive(
		std::vector<std::string>{"glab", "issue", "edit", std::to_string(issue_number)}, path);
}

std::expected<std::string, std::string> GitLabInterface::issue_view(const std::filesystem::path& path, int issue_number)
{
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "issue", "view", std::to_string(issue_number)}, path);
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

std::expected<std::string, std::string> GitLabInterface::issue_close(const std::filesystem::path& path,
																	 int issue_number)
{
	std::expected result =
		SystemInterface::run(std::vector<std::string>{"glab", "issue", "close", std::to_string(issue_number)}, path);
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
