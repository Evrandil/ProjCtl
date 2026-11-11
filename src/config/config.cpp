#include "config.hpp"

#include "config_parsing.hpp"
#include "ini.hpp"
#include "row.hpp"

#include <expected>
#include <filesystem>
#include <format>
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
	Config     config;
	const bool found = Ini::read(path, [&](std::string_view section, std::string_view key, std::string_view value) {
		const ConfigParsing::Section kind =
			ConfigParsing::lookup(ConfigParsing::SECTIONS, section).value_or(ConfigParsing::Section::Unknown);
		ConfigParsing::apply_setting(config, kind, key, value);
	});
	if (!found) {
		return std::unexpected(std::format("Failed to open config file: {}", path.string()));
	}
	return config;
}
