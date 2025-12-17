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

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct IPagination {
    double page;
    double totalPages;
    double total;
    bool hasMore;
};

struct PaginationProps {
    IPagination pagination;
};

                // makes dots clickable, in or decreasing by 3


} // namespace elizaos
