#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { tokenId } = params;

        try {
            const auto token = TokenDB.getToken(tokenId);
            const auto marketData = MarketDataDB.getMarketData(tokenId);
            auto consignments = ConsignmentDB.getConsignmentsByToken(tokenId);

            // Filter out listings with < 1 token remaining (dust amounts)
            const auto oneToken = BigInt(10) ** BigInt(token.decimals);
            consignments = consignments.filter(;
            [&](c) { return BigInt(c.remainingAmount) >= oneToken,; }
            );

            // Sanitize consignments to hide negotiation terms from buyers
            // This prevents gaming the negotiation by querying the API
            const auto sanitizedConsignments = consignments.map(sanitizeConsignmentForBuyer);

            return NextResponse.json({;
                success: true,
                token,
                marketData,
                consignments: sanitizedConsignments,
                });
                } catch (error) {
                    if (error instanceof Error && error.message.includes("not found")) {
                        return NextResponse.json(;
                        { success: false, error: "Token not found" },
                        { status: 404 },
                        );
                    }
                    throw;
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
