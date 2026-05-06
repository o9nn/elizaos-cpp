#include "tokenQuery.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

TokenQueryParams parseTokensQuery(RawTokenQuery raw) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto page = raw.page ? parseInt(raw.page, 10) : 1;
        if (isNaN(page) || page < 1) {
            throw std::runtime_error(`Invalid 'page' parameter: ${raw.page}`);
        }

        const auto limit = raw.limit ? parseInt(raw.limit, 10) : 50;
        if (isNaN(limit) || limit < 1) {
            throw std::runtime_error(`Invalid 'limit' parameter: ${raw.limit}`);
        }
        auto status: TokenStatus | std::nullopt;
        if (raw.status) {
            if (Object.values(TokenStatus).includes(raw.status as TokenStatus)) {
                status = raw.status;
                } else {
                    throw std::runtime_error(`Invalid 'status' parameter: ${raw.status}`);
                }
            }

            auto hideImported: 0 | 1 | std::nullopt;
            if (raw.hideImported == "0") hideImported = 0;
            else if (raw.hideImported == "1") hideImported = 1;

            auto creator: string | std::nullopt;
            if (raw.creator) {
                if (parseSolanaAddress(raw.creator)) {
                    creator = raw.creator;
                    } else {
                        throw std::runtime_error(`Invalid Solana address for 'creator': ${raw.creator}`);
                    }
                }

                const auto search =;
                raw.search && raw.search.trim() != "" ? raw.search.trim() : std::nullopt;

                SortBy sortBy = SortBy.CreatedAt;
                if (raw.sortBy) {
                    if (Object.values(SortBy).includes(raw.sortBy as SortBy)) {
                        sortBy = raw.sortBy;
                        } else {
                            std::cout << "ewa" << std::endl;
                            throw std::runtime_error(`Invalid 'sortBy' parameter: ${raw.sortBy}`);
                        }
                    }

                    SortOrder sortOrder = SortOrder.Desc;
                    if (raw.sortOrder) {
                        const auto lower = raw.sortOrder.toLowerCase();
                        if (lower == SortOrder.Asc || lower == SortOrder.Desc) {
                            sortOrder = lower;
                            } else {
                                throw std::runtime_error(`Invalid 'sortOrder' parameter: ${raw.sortOrder}`);
                            }
                        }

                        return {
                            page,
                            limit,
                            status,
                            hideImported,
                            creator,
                            search,
                            sortBy,
                            sortOrder,
                            };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
