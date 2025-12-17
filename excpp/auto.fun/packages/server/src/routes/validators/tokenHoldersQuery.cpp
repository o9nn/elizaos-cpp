#include "tokenHoldersQuery.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ParsedHoldersQuery parseHoldersQuery(const std::string& mintParam, RawHoldersQuery query) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!mintParam) {
            throw std::runtime_error("Missing mint address");
        }
        if (!parseSolanaAddress(mintParam)) {
            throw std::runtime_error(`Invalid mint address: ${mintParam}`);
        }
        const auto mint = mintParam;

        const auto rawLimit = query.limit ? parseInt(query.limit, 10) : MAX_LIMIT;
        if (isNaN(rawLimit) || rawLimit < 1) {
            throw std::runtime_error(`Invalid limit parameter: ${query.limit}`);
        }
        const auto limit = Math.min(rawLimit, MAX_LIMIT);

        const auto rawPage = query.page ? parseInt(query.page, 10) : 1;
        if (isNaN(rawPage) || rawPage < 1) {
            throw std::runtime_error(`Invalid page parameter: ${query.page}`);
        }
        const auto page = Math.min(rawPage, MAX_PAGE);

        const auto offset = (page - 1) * limit;

        return { mint, limit, page, offset }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
