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

enum TokenStatus {
  Active = "active",
  Migrating = "migrating",
  Migrated = "migrated",
  Locked = "locked",
  Finalized = "finalized",
}

struct Pagination {
    double limit;
    double page;
    double offset;
};

enum SortBy {
  CreatedAt = "createdAt",
  MarketCapUSD = "marketCapUSD",
  Volume24h = "volume24h",
  HolderCount = "holderCount",
  CurveProgress = "curveProgress",
  Featured = "featured",
  Verified = "verified",
}

enum FilterBy {
  Verified = "verified",
}

enum SortOrder {
  Asc = "asc",
  Desc = "desc",
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
    std::optional<0 | 1> hideImported;
    std::optional<std::string> creator;
    std::optional<std::string> search;
    SortBy sortBy;
    SortOrder sortOrder;
};

std::string parseSolanaAddress(unknown raw, auto name = "address");

Pagination parsePaginationQuery(Record<string query, auto string | undefined>, auto { defaultLimit = 50, auto maxLimit = 50, auto maxPage = 1000 } = {});

} // namespace elizaos
