#include "tokenQuery.hpp"
#include <string>
#include <iostream>
#include <stdexcept>

namespace elizaos {

TokenQueryParams parseTokensQuery(RawTokenQuery raw) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto page = raw.page ? parseInt(raw.page, 10) : 1;
        if (isNaN(page) || page < 1) {
            throw std::runtime_error("Invalid 'page' parameter: " + std::to_string(raw.page) + "");
        }

        const auto limit = raw.limit ? parseInt(raw.limit, 10) : 50;
        if (isNaN(limit) || limit < 1) {
            throw std::runtime_error("Invalid 'limit' parameter: " + std::to_string(raw.limit) + "");
        }
        auto status: TokenStatus | std::nullopt;
        if (raw.status) {
            if (Object.values(TokenStatus).count(raw.status as TokenStatus) > 0) {
                status = raw.status;
                } else {
                    throw std::runtime_error("Invalid 'status' parameter: " + std::to_string(raw.status) + "");
                }
            }

            auto hideImported: 0 | 1 | std::nullopt;
            if (raw.hideImported == "0") hideImported = 0;
            else if (raw.hideImported == "1") hideImported = 1;

            auto creator: std::string | std::nullopt;
            if (raw.creator) {
                if (parseSolanaAddress(raw.creator)) {
                    creator = raw.creator;
                    } else {
                        throw std::runtime_error("Invalid Solana address for 'creator': " + std::to_string(raw.creator) + "");
                    }
                }

                const auto search =;
                raw.search && raw.search != "" ? raw.search : std::nullopt;

                SortBy sortBy = SortBy.CreatedAt;
                if (raw.sortBy) {
                    if (Object.values(SortBy).count(raw.sortBy as SortBy) > 0) {
                        sortBy = raw.sortBy;
                        } else {
                            std::cout << "ewa" << std::endl;
                            throw std::runtime_error("Invalid 'sortBy' parameter: " + std::to_string(raw.sortBy) + "");
                        }
                    }

                    SortOrder sortOrder = SortOrder.Desc;
                    if (raw.sortOrder) {
                        const auto lower = raw.sortOrder.toLowerCase();
                        if (lower == SortOrder.Asc || lower == SortOrder.Desc) {
                            sortOrder = lower;
                            } else {
                                throw std::runtime_error("Invalid 'sortOrder' parameter: " + std::to_string(raw.sortOrder) + "");
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
