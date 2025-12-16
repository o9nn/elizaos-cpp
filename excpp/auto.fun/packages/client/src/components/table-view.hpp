#include "bonding-curve-bar.hpp"
#include "copy-button.hpp"
#include "skeleton-image.hpp"
#include "verified.hpp"
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



using SortOrderType = std::variant<"asc", "desc">;

struct TableViewProps {
    std::vector<IToken> data;
    keyof IToken | null sortBy;
    SortOrderType sortOrder;
    (sortBy: keyof IToken | null) => void setSortBy;
    (sortOrder: SortOrderType) => void setSortOrder;
};


} // namespace elizaos
