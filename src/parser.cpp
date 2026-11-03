#include "parser.hpp"


// TODO put implicitly "." as name whenever name isn't specified and makes sense

Command parser(std::span<const std::string_view> args) {
	const std::string_view command = args.at(0);

	if (command == "git") {
		return Cmd::GH::Status { };
	}

	if (command == "repos") {
		return Cmd::GH::RepoList { };
	}



	if (command == "list") {
		try {
			if (args.at(1) == "gits") {
				return Cmd::Base::List { true };
			}
		}
		catch (const std::out_of_range &err) {
			return Cmd::Base::List { false };
		}
		return Cmd::Unknown { };
	}


	std::string name;

	if (args.size() < 2) {
		name = ".";
	} else {
		name = std::string { *args.rbegin() };
	}


	if (command == "path") {
		return Cmd::Base::Path { name };
	}

	if (command == "add") {
		if (name == ".") {
			return Cmd::Base::Add {
				name,
				std::filesystem::path { "." },
			};
		}
		if (args.size() < 3) {
			return Cmd::Unknown { };
		}
		return Cmd::Base::Add { std::string { args.at(1) }, std::filesystem::path { args.at(2) } };
	}

	if (command == "remove" || command == "rm") {
		return Cmd::Base::Remove { name };
	}

	if (command == "open") {
		return Cmd::Base::Open { name };
	}

	if (command == "build") {
		return Cmd::Base::Build { name };
	}

	if (command == "run") {
		return Cmd::Base::Run { name };
	}



	if (command == "status") {
		return Cmd::Git::Status { name };
	}

	if (command == "commit") {
		if (args.size() < 3) {
			return Cmd::Git::Commit {
				name,
				std::nullopt,
			};
		}
		return Cmd::Git::Commit {
			name,
			std::string { args.at(1) },
		};
	}

	if (command == "push") {
		if (args.size() > 2 && args.at(1) == "branch") {
			return Cmd::Git::Push {
				name,
				true,
			};
		}
		return Cmd::Git::Push {
			name,
			false,
		};
	}

	if (command == "pull") {
		return Cmd::Git::Pull { name };
	}

	if (command == "fetch") {
		return Cmd::Git::Fetch { name };
	}

	if (command == "branch") {
		if (args.size() < 3) {
			return Cmd::Git::Branch {
				name,
				std::nullopt,
			};
		}
		return Cmd::Git::Branch {
			name,
			std::string { args.at(1) },
		};
	}



	if (command == "merge") {
		return Cmd::Git::Merge { name };
	}

	if (command == "issues" || command == "ilist") {
		return Cmd::GH::IssueList { name };
	}

	if (command == "icreate") {
		return Cmd::GH::IssueCreate { name };
	}


	if (command == "reqs" || command == "reqlist") {
		return Cmd::GH::RequestList { name };
	}

	if (command == "reqcreate") {
		return Cmd::GH::RequestCreate { name };
	}

	if (command == "reqmerge") {
		return Cmd::GH::RequestMerge { name };
	}

	return Cmd::Unknown { };
}
