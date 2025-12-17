#pragma once
#include <algorithm>
#include <any>
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

enum TokenStatus {
}

struct Pagination {
    double limit;
    double page;
    double offset;
};

enum SortBy {
}

enum FilterBy {
}

enum SortOrder {
}

struct RawTokenQuery {
    std::optional<std::string> page;
    std::optional<std::string> limit;
    std::optional<std::string> status;
    std::optional<std::string> hideImported;
    std::optional<std::string> creator;
    std::optional<std::string> search;
    std::optional<std::string> sortBy;
    std::optional<std::string> sortOrder;
};

struct TokenQueryParams {
    double page;
    double limit;
    std::optional<TokenStatus> status;
    std::optional<int> hideImported;
    std::optional<std::string> creator;
    std::optional<std::string> search;
    SortBy sortBy;
    SortOrder sortOrder;
};

std::string parseSolanaAddress(const std::any& raw, auto name);


} // namespace elizaos
