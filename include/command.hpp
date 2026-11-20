#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <variant>

namespace Cmd {

	struct Version {};

	struct Unknown {
		std::string message;
	};

	namespace Base {

		struct List {
			bool gits_only  = false;
			bool names_only = false;
		};

		struct Path {
			std::optional<std::string> name;
		};

		struct Add {
			std::optional<std::string>           name;
			std::optional<std::filesystem::path> path;
		};

		struct Remove {
			std::string name;
		};

		struct Open {
			std::optional<std::string> name;
		};

		struct Build {
			std::optional<std::string> name;
		};

		struct Run {
			std::optional<std::string> name;
		};

	} // namespace Base

	namespace Git {

		struct Status {
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct Commit {
			bool                       tracked_only = false;
			std::optional<std::string> name;
			std::optional<std::string> message;
		};

		struct Push {
			bool                       branch_upstream = false;
			std::optional<std::string> name;
		};

		struct Pull {
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct Fetch {
			bool                       all = false;
			std::optional<std::string> name;
		};

		struct Branch {
			std::optional<std::string> name;
			std::optional<std::string> branch_name;
		};

		struct Merge {
			std::optional<std::string> name;
		};

		struct Log {
			bool                       oneline = false;
			std::optional<std::string> name;
		};

		struct Diff {
			std::optional<std::string>           name;
			std::optional<std::filesystem::path> path;
		};

	} // namespace Git

	// TODO Split that to Issue and Request subnamespaces?
	namespace GH {

		struct Status {};

		struct RepoList {};

		namespace Issue {

			struct List {
				bool                       all = false;
				std::optional<std::string> name;
			};

			struct Create {
				std::optional<std::string> name;
			};

			struct Edit {
				int                        number = 0;
				std::optional<std::string> name;
			};

			struct View {
				int                        number = 0;
				std::optional<std::string> name;
			};

			struct Close {
				int                        number = 0;
				std::optional<std::string> name;
			};

		} // namespace Issue

		namespace Request {

			struct List {
				bool                       all = false;
				std::optional<std::string> name;
			};

			struct Create {
				std::optional<std::string> name;
			};

			struct Edit {
				int                        number = 0;
				std::optional<std::string> name;
			};

			struct View {
				int                        number = 0;
				std::optional<std::string> name;
			};

			struct Close {
				int                        number = 0;
				std::optional<std::string> name;
			};

			struct Merge {
				std::optional<std::string> name;
			};

		} // namespace Request

	} // namespace GH

} // namespace Cmd

// clang-format off
using Command = std::variant<
	Cmd::Unknown,
	Cmd::Version,
	Cmd::Base::List,
	Cmd::Base::Path,
	Cmd::Base::Add,
	Cmd::Base::Remove,
	Cmd::Base::Open,
	Cmd::Base::Build,
	Cmd::Base::Run,
	Cmd::Git::Status,
	Cmd::Git::Commit,
	Cmd::Git::Push,
	Cmd::Git::Pull,
	Cmd::Git::Fetch,
	Cmd::Git::Branch,
	Cmd::Git::Merge,
	Cmd::Git::Log,
	Cmd::Git::Diff,
	Cmd::GH::Status,
	Cmd::GH::RepoList,
	Cmd::GH::Issue::List,
	Cmd::GH::Issue::Create,
	Cmd::GH::Issue::Edit,
	Cmd::GH::Issue::View,
	Cmd::GH::Issue::Close,
	Cmd::GH::Request::List,
	Cmd::GH::Request::Create,
	Cmd::GH::Request::Edit,
	Cmd::GH::Request::View,
	Cmd::GH::Request::Close,
	Cmd::GH::Request::Merge
	>;
// clang-format on
