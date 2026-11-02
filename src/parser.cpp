#include "parser.hpp"


Command parser(std::span<const std::string_view> args) {
	const std::string_view command = args.at(0);

	if (command == "list") {
		bool gits_only = false;
		if (args.size() > 1 && args.at(1) == "gits") {
			gits_only = true;
		}
		return Cmd::List { gits_only } ;
	}

	if (command == "path") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Path { std::string { args.at(1) } } ;
	}

	if (command == "add") {
		if (args.at(1) == ".") {
			return Cmd::Add { std::string { "." }, std::filesystem::path { "." } } ;
		}
		if (args.size() < 3) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Add { std::string { args.at(1) }, std::filesystem::path { args.at(2) } } ;
	}

	if (command == "remove" || command == "rm") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Remove { std::string { args.at(1) } } ;
	}

	if (command == "open") {
		if (args.size() < 2) {
			return Cmd::Unknown { } ;
		}
		return Cmd::Open { std::string { args.at(1) } } ;
	}

	return Cmd::Unknown { } ;
}
