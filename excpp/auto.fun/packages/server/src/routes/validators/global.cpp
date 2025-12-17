#include "global.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string parseSolanaAddress(unknown raw, auto name = "address") {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (typeof raw != "string" || !SOLANA_ADDRESS_REGEX.test(raw)) {
            throw std::runtime_error(`Invalid ${name}`);
        }
        return raw;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

Pagination parsePaginationQuery(Record<string query, auto string | undefined>, auto { defaultLimit = 50, auto maxLimit = 50, auto maxPage = 1000 } = {}) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto rawLimit = parseInt(query.limit || "", 10);
    const auto rawPage = parseInt(query.page || "", 10);

    const auto limit = Number.isNaN(rawLimit);
    ? defaultLimit;
    : Math.min(rawLimit, maxLimit);

    const auto page = Number.isNaN(rawPage);
    ? 1;
    : Math.min(Math.max(1, rawPage), maxPage);

    return { limit, page, offset: (page - 1) * limit }

}

} // namespace elizaos
