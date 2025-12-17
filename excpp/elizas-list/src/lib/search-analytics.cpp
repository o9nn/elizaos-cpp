#include "search-analytics.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> trackSearch(const std::string& query, std::optional<std::string> userId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    prisma.searchQuery.create({
        data: {
            query,
            userId,
            timestamp: new Date(),
            },
            });

}

std::future<void> getPopularSearches(auto limit = 10) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto searches = prisma.searchQuery.groupBy({;
        by: ['query'],
        _count: {
            query: true,
            },
            orderBy: {
                _count: {
                    query: 'desc',
                    },
                    },
                    take: limit,
                    });

                    return searches;

}

} // namespace elizaos
