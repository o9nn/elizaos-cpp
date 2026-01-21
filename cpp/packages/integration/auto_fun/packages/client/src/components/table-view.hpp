#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "bonding-curve-bar.hpp"
#include "copy-button.hpp"
#include "skeleton-image.hpp"
#include "verified.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using SortOrderType = std::string;

struct TableViewProps {
    std::vector<IToken> data;
    std::optional<keyof IToken> sortBy;
    SortOrderType sortOrder;
};

void TableView(auto sortBy, auto sortOrder, auto setSortBy, auto setSortOrder);

} // namespace elizaos
