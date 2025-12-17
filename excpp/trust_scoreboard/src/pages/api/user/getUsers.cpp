#include "getUsers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> handler(NextApiRequest req, NextApiResponse res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Get cursor and limit from query params, with defaults
        const auto cursor = parseInt(req.query.cursor) || 1;
        const auto limit = parseInt(req.query.limit) || 100;

        // Simulate pagination (though we're using mock data)
        const auto startIndex = (cursor - 1) * limit;
        const auto endIndex = startIndex + limit;
        const auto paginatedUsers = mockUsers.slice(startIndex, endIndex);

        std::cout << "Returning " + std::to_string(paginatedUsers.length) + " mock users for trust scoreboard" << std::endl;

        return res.status(200).json({;
            users: paginatedUsers,
            total: mockUsers.length,
            cursor: cursor,
            hasMore: endIndex < mockUsers.length
            });
            } catch (error) {
                std::cerr << "Error:" << error << std::endl;
                return res.status(500).json({ error: "Internal Server Error" });
            }

}

} // namespace elizaos
