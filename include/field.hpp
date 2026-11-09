#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

enum class TableField : uint8_t {
	Number,
	Title,
	State,
	Author,
	Labels,
	Created,
	Updated,
	Parent,
};

struct Field {
	int                        number = 0;
	std::string                title;
	std::string                state;
	std::string                author;
	std::string                created;
	std::string                updated;
	std::vector<std::string>   labels;
	std::optional<std::string> parent;
};
