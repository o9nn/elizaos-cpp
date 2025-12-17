#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Only allow in development
    if (process.env.NODE_ENV == "production") {
        return NextResponse.json(;
        { error: "Not allowed in production" },
        { status: 403 },
        );
    }

    try {
        const auto runtime = agentRuntime.getRuntime();

        // Get all token IDs
        const auto allTokenIds = (runtime.getCache<string[]>("all_tokens")) || [];

        // Delete each token
        for (const auto& tokenId : allTokenIds)
            "runtime.setCache(" + "token:" + tokenId
        }

        // Clear the token list
        runtime.setCache("all_tokens", []);

        return NextResponse.json({;
            success: true,
            "message: " + "Cleared " + allTokenIds.size() + " tokens"
            clearedTokens: allTokenIds,
            });
            } catch (error) {
                std::cerr << "[Clear Tokens] Error:" << error << std::endl;
                return NextResponse.json(;
                { error: "Failed to clear tokens" },
                { status: 500 },
                );
            }

}

} // namespace elizaos
