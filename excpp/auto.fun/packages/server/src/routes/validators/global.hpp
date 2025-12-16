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


const MAX_LIMIT = 50;
const MAX_PAGE = 500;

const SOLANA_ADDRESS_REGEX = /^[1-9A-HJ-NP-Za-km-z]{32,44}$/;

`);
  }
  return raw;
}

 = {}
): Pagination {
  const rawLimit = parseInt(query.limit || "", 10);
  const rawPage = parseInt(query.page || "", 10);

  const limit = Number.isNaN(rawLimit)
    ? defaultLimit
    : Math.min(rawLimit, maxLimit);

  const page = Number.isNaN(rawPage)
    ? 1
    : Math.min(Math.max(1, rawPage), maxPage);

  return { limit, page, offset: (page - 1) * limit };
}

} // namespace elizaos
