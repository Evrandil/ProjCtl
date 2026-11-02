#pragma once

#include <variant>
#include <string>
#include <filesystem>

namespace Cmd {

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

}

using Command = std::variant<
	Cmd::Unknown,
	Cmd::List,
	Cmd::Path,
	Cmd::Add,
	Cmd::Remove,
	Cmd::Open,
	Cmd::Build
>;
