#pragma once

#include <variant>
#include <string>

namespace Cmd {

	struct Unknown {
	};

	struct List {
		bool gits_only;
	};

	struct Path {
		std::string name;
	};

}

using Command = std::variant<
	Cmd::Unknown,
	Cmd::List,
	Cmd::Path
	>;
