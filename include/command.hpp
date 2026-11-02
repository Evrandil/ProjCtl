#pragma once

#include <variant>
#include <string>
#include <filesystem>

namespace Cmd {

	namespace Base {

		struct Unknown {
		};

		struct List {
			bool gits_only;
		};

		struct Path {
			std::string name;
		};

		struct Add {
			std::string name;
			std::filesystem::path path;
		};

		struct Remove {
			std::string name;
		};

		struct Open {
			std::string name;
		};

		struct Build {
			std::string name;
		};

		struct Run {
			std::string name;
		};

	}

	namespace Git {
		struct Status {
			std::string name;
		};

	}

}

using Command = std::variant<
	Cmd::Base::Unknown,
	Cmd::Base::List,
	Cmd::Base::Path,
	Cmd::Base::Add,
	Cmd::Base::Remove,
	Cmd::Base::Open,
	Cmd::Base::Build,
	Cmd::Base::Run,
	Cmd::Git::Status
>;
