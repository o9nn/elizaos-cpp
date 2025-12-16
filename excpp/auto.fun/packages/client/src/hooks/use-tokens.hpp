#include "use-pagination.hpp"
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



using Token = z.infer<typeof TokenSchema>;

using SortOrderType = std::variant<"asc", "desc">;

struct UseTokensParams {
    keyof IToken | "featured" sortBy;
    SortOrderType sortOrder;
    std::optional<double> hideImported;
    std::optional<"all" | "active" | "locked"> status;
    std::optional<double> pageSize;
    std::optional<bool> enabled;
};

  /** Prepopulate token cache, that we otherwise need to fetch first on the /token page */

        // Only update if token has a ticker


} // namespace elizaos
