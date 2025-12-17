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



// Interface matching the PreGeneratedToken from the database
struct PreGeneratedToken {
    std::string id;
    std::string name;
    std::string ticker;
    std::string description;
    std::string prompt;
    std::string image;
    std::string createdAt;
    double used;
};

// Interface for pagination response
struct PaginatedTokensResponse {
    std::vector<PreGeneratedToken> tokens;
    double page;
    double totalPages;
    double total;
    bool hasMore;
};


} // namespace elizaos
