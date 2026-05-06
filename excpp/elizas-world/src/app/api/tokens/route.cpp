#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Check cache first
        const auto cached = getCachedData();
        if (cached && !shouldRefreshCache()) {
            return new NextResponse(/* JSON.stringify */ std::string(cached.holdings), {;
                headers: {
                    "Content-Type": "application/json",
                    "Cache-Control": "public, max-age=60",
                    },
                    });
                }

                // Fetch fresh data
                const auto tokenBalances = getTokenData(connection);
                const auto marketData = fetchDexScreenerData(;
                tokenBalances.map(t => t.mint);
                );
                const auto holdings = calculateHoldings(;
                connection,
                tokenBalances,
                marketData,
                WALLET_ADDRESS;
                );

                // Update cache
                setCachedData(holdings);

                return new NextResponse(/* JSON.stringify */ std::string(holdings), {;
                    headers: {
                        "Content-Type": "application/json",
                        "Cache-Control": "public, max-age=60",
                        },
                        });
                        } catch (error) {
                            std::cerr << "API Error:" << error << std::endl;
                            // Return cached data on error if available
                            const auto cached = getCachedData();
                            if (cached) {
                                return new NextResponse(/* JSON.stringify */ std::string(cached.holdings), {;
                                    headers: {
                                        "Content-Type": "application/json",
                                        "Cache-Control": "public, max-age=60",
                                        },
                                        });
                                    }
                                    return NextResponse.error();
                                }

}

} // namespace elizaos
