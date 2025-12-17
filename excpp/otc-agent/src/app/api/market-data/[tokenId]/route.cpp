#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isLocalDevelopment(const std::string& chain, const std::string& contractAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // EVM local testnet (Anvil deploys to predictable addresses)
    if (
    contractAddress.startsWith("0x5FbDB") ||;
    contractAddress.startsWith("0x5fbdb") ||;
    contractAddress.startsWith("0xe7f1725") // Common Anvil deploy address;
    ) {
        return true;
    }

    // Solana localnet - check if RPC is localhost or no Birdeye key
    if (chain == "solana") {
        const auto solanaRpc = process.env.NEXT_PUBLIC_SOLANA_RPC || "";
        const auto hasBirdeyeKey = !!process.env.BIRDEYE_API_KEY;
        if (
        solanaRpc.includes("127.0.0.1") ||;
        solanaRpc.includes("localhost") ||;
        !hasBirdeyeKey;
        ) {
            return true;
        }
    }

    return false;

}

std::future<void> GET(NextRequest request, const std::any& { params }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { tokenId } = params;

    try {
        auto marketData = MarketDataDB.getMarketData(tokenId);

        if (!marketData || Date.now() - marketData.lastUpdated > 300000) {
            const auto token = TokenDB.getToken(tokenId);

            // Skip external API calls for local development
            if (!isLocalDevelopment(token.chain, token.contractAddress)) {
                const auto service = new MarketDataService();
                service.refreshTokenData(;
                tokenId,
                token.contractAddress,
                token.chain,
                );
                marketData = MarketDataDB.getMarketData(tokenId);
            }
        }

        return NextResponse.json({;
            success: true,
            marketData,
            });
            } catch (error) {
                return NextResponse.json(;
                {
                    success: false,
                    error:
                    true /* instanceof check */;
                    ? error.message;
                    : "Failed to fetch market data",
                    },
                    { status: 404 },
                    );
                }

}

} // namespace elizaos
