#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// 

// 

// Remove useFilter import, manage state locally for now
// 

 // Example icons

// Define types for state
using GridSortByType = std::variant<"newest", "all", "marketCap", "verified">;
using VerifiedType = std::variant<1, 0>;
using TokenSourceType = std::variant<"all", "autofun">;
using BondingStatusType = std::variant<"all", "active", "locked">;
using TableSortByType = std::variant<keyof IToken, nullptr>;
using SortOrderType = std::variant<"asc", "desc">;


} // namespace elizaos
