#pragma once

#include <filesystem>
#include <string>
#include <variant>

namespace Cmd
{

	struct Version
	{
	};

	struct Unknown
	{
		std::string message;
	};

	namespace Base
	{

		struct List
		{
			bool gits_only  = false;
			bool names_only = false;
		};

		struct Path
		{
			std::optional<std::string> name;
		};

		struct Add
		{
			std::optional<std::string>           name;
			std::optional<std::filesystem::path> path;
		};

		struct Remove
		{
			std::string name;
		};

		struct Open
		{
			std::optional<std::string> name;
		};

		struct Build
		{
			std::optional<std::string> name;
		};

		struct Run
		{
			std::optional<std::string> name;
		};

	} // namespace Base

	namespace Git
	{

		struct Status
		{
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct Commit
		{
			std::optional<std::string> name;
			std::optional<std::string> message;
		};

		struct Push
		{
			bool                       branch_upstream = false;
			std::optional<std::string> name;
		};

		struct Pull
		{
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct Fetch
		{
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct Branch
		{
			std::optional<std::string> name;
			std::optional<std::string> branch_name;
		};

		struct Merge
		{
			std::optional<std::string> name;
		};

		struct Log
		{
			bool                       oneline = false;
			std::optional<std::string> name;
		};

		struct Diff
		{
			std::optional<std::string>           name;
			std::optional<std::filesystem::path> path;
		};

	} // namespace Git

	// TODO Split that to Issue and Request subnamespaces?
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
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct IssueCreate
		{
			std::optional<std::string> name;
		};

		struct IssueEdit
		{
			int                        number = 0;
			std::optional<std::string> name;
		};

		struct IssueView
		{
			int                        number = 0;
			std::optional<std::string> name;
		};

		struct IssueClose
		{
			int                        number = 0;
			std::optional<std::string> name;
		};

		struct RequestList
		{
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct RequestCreate
		{
			std::optional<std::string> name;
		};

		struct RequestEdit
		{
			int                        number = 0;
			std::optional<std::string> name;
		};

		struct RequestView
		{
			int                        number = 0;
			std::optional<std::string> name;
		};

		struct RequestClose
		{
			int                        number = 0;
			std::optional<std::string> name;
		};

		struct RequestMerge
		{
			std::optional<std::string> name;
		};

	} // namespace GH

} // namespace Cmd

using Command =
	std::variant<Cmd::Unknown, Cmd::Version, Cmd::Base::List, Cmd::Base::Path, Cmd::Base::Add, Cmd::Base::Remove, Cmd::Base::Open, Cmd::Base::Build, Cmd::Base::Run, Cmd::Git::Status, Cmd::Git::Commit, Cmd::Git::Push,
	             Cmd::Git::Pull, Cmd::Git::Fetch, Cmd::Git::Branch, Cmd::Git::Merge, Cmd::Git::Log, Cmd::Git::Diff, Cmd::GH::Status, Cmd::GH::RepoList, Cmd::GH::IssueList, Cmd::GH::IssueCreate, Cmd::GH::IssueEdit,
	             Cmd::GH::IssueView, Cmd::GH::IssueClose, Cmd::GH::RequestList, Cmd::GH::RequestCreate, Cmd::GH::RequestEdit, Cmd::GH::RequestView, Cmd::GH::RequestClose, Cmd::GH::RequestMerge>;
