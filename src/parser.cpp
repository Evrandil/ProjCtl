#include "parser.hpp"


Command parser(std::span<const std::string_view> args) {
	const std::string_view command = args.at(0);

	if (command == "list") {
		bool gits_only = false;
		if (args.size() > 1 && args.at(1) == "gits") {
			gits_only = true;
		}
		return Cmd::Base::List { gits_only } ;
	}

	if (command == "path") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Base::Path { std::string { args.at(1) } } ;
	}

	if (command == "add") {
		if (args.at(1) == ".") {
			return Cmd::Base::Add { std::string { "." }, std::filesystem::path { "." } } ;
		}
		if (args.size() < 3) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Base::Add { std::string { args.at(1) }, std::filesystem::path { args.at(2) } } ;
	}

	if (command == "remove" || command == "rm") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Base::Remove { std::string { args.at(1) } } ;
	}

	if (command == "open") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Base::Open { std::string { args.at(1) } } ;
	}

	if (command == "build") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Base::Build { std::string { args.at(1) } } ;
	}

	if (command == "run") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Base::Run { std::string { args.at(1) } } ;
	}

	if (command == "status") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Git::Status { std::string { args.at(1) } } ;
	}

	if (command == "commit") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		if (args.size() < 3) {
			return Cmd::Git::Commit {
				std::string { args.at(1) },
				std::nullopt,
			};
		}
		return Cmd::Git::Commit {
			std::string { args.at(2) },
			std::string { args.at(1) },
		};
	}

	if (command == "push") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		if (args.size() > 2 && args.at(1) == "branch") {
			return Cmd::Git::Push {
				std::string { args.at(2) },
				true,
			};
		}
		return Cmd::Git::Push {
			std::string { args.at(1) },
			false,
		};
	}

	if (command == "pull") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Git::Pull { std::string { args.at(1) } } ;
	}

	if (command == "fetch") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Git::Fetch { std::string { args.at(1) } } ;
	}

	if (command == "branch") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		if (args.size() < 3) {
			return Cmd::Git::Branch {
				std::string { args.at(1) },
				std::nullopt,
			};
		}
		return Cmd::Git::Branch {
			std::string { args.at(2) },
			std::string { args.at(1) },
		};
	}

	if (command == "merge") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Git::Merge { std::string { args.at(1) } } ;
	}

	if (command == "git") {
		return Cmd::GH::Status { } ;
	}


	if (command == "repos") {
		return Cmd::GH::RepoList { } ;
	}

	return Cmd::Unknown { } ;
}
