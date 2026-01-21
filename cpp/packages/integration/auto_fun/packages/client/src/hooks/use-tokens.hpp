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
#include "use-pagination.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using Token = z::infer<typeof TokenSchema>;

using SortOrderType = std::string;

struct UseTokensParams {
    keyof IToken sortBy;
    SortOrderType sortOrder;
    std::optional<double> hideImported;
    std::optional<std::string> status;
    std::optional<double> pageSize;
    std::optional<bool> enabled;
};

  /** Prepopulate token cache, that we otherwise need to fetch first on the /token page */

        // Only update if token has a ticker


} // namespace elizaos
