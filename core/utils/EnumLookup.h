#pragma once

#include <optional>
#include <utility>

// The linear-scan lookup mechanism shared by every enum<->representation
// table in the project (piece letters, animation state folder names,
// piece point values, ...). Only the mechanism lives here - the tables
// themselves (the actual data) stay local to whoever owns that domain
// knowledge, so this file never needs to know what a PieceType or a
// Color is.
template <typename Key, typename Value, size_t N>
std::optional<Value> lookupValue(const std::pair<Key, Value> (&table)[N], const Key& key) {
	for (const auto& entry : table) {
		if (entry.first == key) {
			return entry.second;
		}
	}
	return std::nullopt;
}

template <typename Key, typename Value, size_t N>
std::optional<Key> lookupKey(const std::pair<Key, Value> (&table)[N], const Value& value) {
	for (const auto& entry : table) {
		if (entry.second == value) {
			return entry.first;
		}
	}
	return std::nullopt;
}
