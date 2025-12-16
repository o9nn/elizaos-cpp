#include "global.hpp"
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


;

struct RawHoldersQuery {
    std::optional<std::string> limit;
    std::optional<std::string> page;
};


struct ParsedHoldersQuery {
    std::string mint;
    double limit;
    double page;
    double offset;
};




   if (!parseSolanaAddress(mintParam)) {
      throw new Error(`Invalid mint address: ${mintParam}`);
   }
   const mint = mintParam;

   const rawLimit = query.limit ? parseInt(query.limit, 10) : MAX_LIMIT;
   if (isNaN(rawLimit) || rawLimit < 1) {
      throw new Error(`Invalid limit parameter: ${query.limit}`);
   }
   const limit = Math.min(rawLimit, MAX_LIMIT);

   const rawPage = query.page ? parseInt(query.page, 10) : 1;
   if (isNaN(rawPage) || rawPage < 1) {
      throw new Error(`Invalid page parameter: ${query.page}`);
   }
   const page = Math.min(rawPage, MAX_PAGE);

   const offset = (page - 1) * limit;

   return { mint, limit, page, offset };
}

} // namespace elizaos
