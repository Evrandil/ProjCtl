#pragma once

#include <variant>

namespace Cmd {

	struct Unknown {
	};

	struct List {
		bool gits_only;
	};

}

using Command = std::variant<Cmd::Unknown, Cmd::List>;
