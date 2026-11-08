#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class RequestField : uint8_t {
	Number,
	Title,
	State,
	Author,
	Labels,
	Created,
	Updated,
	Parent,
};

struct Request {
	int                        number = 0;
	std::string                title;
	std::string                state;
	std::string                author;
	std::string                created;
	std::string                updated;
	std::vector<std::string>   labels;
	std::optional<std::string> parent;
};
