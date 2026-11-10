#include "config.hpp"

#include "config_parsing.hpp"
#include "row.hpp"
#include "text.hpp"

#include <cstddef>
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>

auto column_of(const Settings::Tables& tables, TableField field) -> const Settings::Column& {
	switch (field) {
		case TableField::Number:  return tables.number;
		case TableField::Title:   return tables.title;
		case TableField::State:   return tables.state;
		case TableField::Author:  return tables.author;
		case TableField::Labels:  return tables.labels;
		case TableField::Created: return tables.created;
		case TableField::Updated: return tables.updated;
		case TableField::Parent:  return tables.parent;
	}
	std::unreachable();
}

auto load_config(const std::filesystem::path& path) -> std::expected<Config, std::string> {
	std::ifstream config_file(path);
	if (!config_file) {
		return std::unexpected("Failed to open config file: " + path.string());
	}

	Config                 config;
	ConfigParsing::Section section = ConfigParsing::Section::Unknown;
	std::string            line;
	while (std::getline(config_file, line)) {
		const std::string_view trimmed = trim(line);
		if (trimmed.empty() || trimmed.front() == '#' || trimmed.front() == ';') {
			continue;
		}
		if (trimmed.front() == '[' && trimmed.back() == ']') {
			section = lookup(ConfigParsing::SECTIONS, trim(trimmed.substr(1, trimmed.size() - 2)))
			              .value_or(ConfigParsing::Section::Unknown);
			continue;
		}
		if (const std::size_t separator = trimmed.find('='); separator != std::string_view::npos) {
			apply_setting(config, section, trim(trimmed.substr(0, separator)), trim(trimmed.substr(separator + 1)));
		}
	}
	return config;
}
