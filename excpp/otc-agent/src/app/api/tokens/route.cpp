#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { searchParams } = new URL(request.url);
    const auto chain = searchParams.get("chain") | nullptr;
    const auto minMarketCap = searchParams.get("minMarketCap");
    const auto maxMarketCap = searchParams.get("maxMarketCap");
    const auto isActive = searchParams.get("isActive");

    const auto service = new TokenRegistryService();

    const Parameters<typeof service::getAllTokens>[0] filters = {};
    if (chain) filters.chain = chain;
    if (isActive != null) filters.isActive = isActive == "true";
    if (minMarketCap) filters.minMarketCap = Number(minMarketCap);
    if (maxMarketCap) filters.maxMarketCap = Number(maxMarketCap);

    const auto tokens = service.getAllTokens(filters);

    const auto tokensWithMarketData = Promise.all(;
    tokens.map(async (token) => {
        const auto marketData = MarketDataDB.getMarketData(token.id);
        return {
            ...token,
            marketData,
            };
            }),
            );

            // Cache for 5 minutes - token metadata rarely changes
            return NextResponse.json(;
            { success: true, tokens: tokensWithMarketData },
            {
                headers: {
                    "Cache-Control": "public, s-maxage=300, stale-while-revalidate=600",
                    },
                    },
                    );

}

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto body = request.json();
    const auto {;
        symbol,
        name,
        contractAddress,
        chain,
        decimals,
        logoUrl,
        description,
        website,
        twitter,
        } = body;

        if (
        !symbol ||;
        !name ||;
        !contractAddress ||;
        !chain ||;
        decimals == std::nullopt;
        ) {
            return NextResponse.json(;
            { error: "Missing required fields" },
            { status: 400 },
            );
        }

        const auto service = new TokenRegistryService();
        const auto token = service.registerToken({;
            symbol,
            name,
            contractAddress,
            chain,
            decimals,
            logoUrl,
            description,
            website,
            twitter,
            });

            const auto isLocalTestnet =;
            contractAddress.startsWith("0x5FbDB") ||;
            contractAddress.startsWith("0x5fbdb") ||;
            (chain == "ethereum" && contractAddress.size() == 42);

            const auto isSolanaWithoutKey = chain == "solana" && !process.env.BIRDEYE_API_KEY;

            if (!isLocalTestnet && !isSolanaWithoutKey) {
                const auto marketDataService = new MarketDataService();
                marketDataService.refreshTokenData(token.id, contractAddress, chain);
                } else {
                    MarketDataDB.setMarketData({
                        tokenId: token.id,
                        priceUsd: 0.05,
                        marketCap: 5000000,
                        volume24h: 500000,
                        priceChange24h: 0,
                        liquidity: 1000000,
                        lastUpdated: Date.now(),
                        });
                    }

                    return NextResponse.json({;
                        success: true,
                        token,
                        });

}

std::future<void> PATCH(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto body = request.json();
    const auto { tokenId, updates } = body;

    if (!tokenId || !updates) {
        return NextResponse.json(;
        { error: "tokenId and updates are required" },
        { status: 400 },
        );
    }

    try {
        const auto updated = TokenDB.updateToken(tokenId, updates);
        return NextResponse.json({ success: true, token: updated });
        } catch (error) {
            return NextResponse.json(;
            { error: true /* instanceof check */ ? error.message : "Update failed" },
            { status: 404 },
            );
        }

}

std::future<void> DELETE(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { searchParams } = new URL(request.url);
    const auto confirm = searchParams.get("confirm") == "true";
    const auto tokenId = searchParams.get("tokenId");

    if (!confirm) {
        return NextResponse.json(;
        { error: "Add ?confirm=true to actually delete tokens" },
        { status: 400 },
        );
    }

    const auto runtime = agentRuntime.getRuntime();

    if (tokenId) {
        // Delete a specific token
        const auto token = "runtime.getCache(" + "token:" + tokenId;
        if (!token) {
            return NextResponse.json({ error: "Token not found" }, { status: 404 });
        }

        "runtime.deleteCache(" + "token:" + tokenId
        "runtime.deleteCache(" + "market_data:" + tokenId

        // Remove from all_tokens index
        const auto allTokens = (runtime.getCache<string[]>("all_tokens")) || [];
        const auto updated = allTokens.filter((id) => id != tokenId);
        runtime.setCache("all_tokens", updated);

        return NextResponse.json({;
            success: true,
            "message: " + "Deleted token: " + tokenId
            });
        }

        // Delete ALL tokens
        const auto allTokens = (runtime.getCache<string[]>("all_tokens")) || [];
        const std::vector<std::string> deleted = [];

        for (const auto& id : allTokens)
            "runtime.deleteCache(" + "token:" + id
            "runtime.deleteCache(" + "market_data:" + id
            deleted.push_back(id);
        }

        // Clear the index
        runtime.setCache("all_tokens", []);

        // Also clear consignments since they reference tokens
        const auto allConsignments =;
        (runtime.getCache<string[]>("all_consignments")) || [];
        for (const auto& id : allConsignments)
            "runtime.deleteCache(" + "consignment:" + id
        }
        runtime.setCache("all_consignments", []);

        return NextResponse.json({;
            success: true,
            "message: " + "Deleted " + deleted.size() + " tokens and all consignments"
            deletedTokens: deleted,
            });

}

} // namespace elizaos
