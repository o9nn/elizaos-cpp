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
    std::optional<std:> page;
    std::optional<std:> limit;
    std::optional<std:> status;
    std::optional<std:> hideImported;
    std::optional<std:> creator;
    std::optional<std:> search;
    std::optional<std:> sortBy;
    std::optional<std:> sortOrder;
};

struct TokenQueryParams {
    double page;
    double limit;
    std::optional<TokenStatus> status;
    std::optional<0 | 1> hideImported;
    std::optional<std:> creator;
    std::optional<std:> search;
    SortBy sortBy;
    SortOrder sortOrder;
};

std: parseSolanaAddress(unknown raw, auto name = "address");

Pagination parsePaginationQuery(Record<std: query, auto std: | undefined>, auto { defaultLimit = 50, auto maxLimit = 50, auto maxPage = 1000 } = {});

} // namespace elizaos
