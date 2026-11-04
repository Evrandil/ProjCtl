#pragma once

#include <variant>
#include <string>
#include <filesystem>


namespace Cmd
{

	struct Unknown
	{
	};

	namespace Base
	{

		struct List
		{
			bool gits_only;
		};

		struct Path
		{
			std::string name;
		};

		struct Add
		{
			std::string name;
			std::filesystem::path path;
		};

		struct Remove
		{
			std::string name;
		};

		struct Open
		{
			std::string name;
		};

		struct Build
		{
			std::string name;
		};

		struct Run
		{
			std::string name;
		};

	} // namespace Base

	namespace Git
	{

		struct Status
		{
			bool all;
			std::string name;
		};

		struct Commit
		{
			std::string name;
			std::optional<std::string> message;
		};

		struct Push
		{
			bool branch_upstream;
			std::string name;
		};

		struct Pull
		{
			bool all;
			std::string name;
		};

		struct Fetch
		{
			bool all;
			std::string name;
		};

		struct Branch
		{
			std::string name;
			std::optional<std::string> branch_name;
		};

		struct Merge
		{
			std::string name;
		};

	} // namespace Git

	namespace GH
	{

		struct Status
		{
		};

		struct RepoList
		{
		};

		struct IssueList
		{
			bool all;
			std::string name;
		};

		struct IssueCreate
		{
			std::string name;
		};

		struct RequestList
		{
			bool all;
			std::string name;
		};

		struct RequestCreate
		{
			std::string name;
		};

		struct RequestMerge
		{
			std::string name;
		};

	} // namespace GH

} // namespace Cmd


using Command = std::variant<Cmd::Unknown, Cmd::Base::List, Cmd::Base::Path, Cmd::Base::Add, Cmd::Base::Remove, Cmd::Base::Open, Cmd::Base::Build, Cmd::Base::Run, Cmd::Git::Status, Cmd::Git::Commit, Cmd::Git::Push, Cmd::Git::Pull, Cmd::Git::Fetch, Cmd::Git::Branch, Cmd::Git::Merge, Cmd::GH::Status,
							 Cmd::GH::RepoList, Cmd::GH::IssueList, Cmd::GH::IssueCreate, Cmd::GH::RequestList, Cmd::GH::RequestCreate, Cmd::GH::RequestMerge>;
