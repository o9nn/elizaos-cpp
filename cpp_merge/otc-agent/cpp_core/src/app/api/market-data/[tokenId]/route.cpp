#include "route.hpp"
#include <string>
#include <future>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isLocalDevelopment(const std::string& chain, const std::string& contractAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // EVM local testnet (Anvil deploys to predictable addresses)
    if (
    contractAddress.substr(0, "0x5FbDB") ||;
    contractAddress.substr(0, "0x5fbdb") ||;
    contractAddress.substr(0, "0xe7f1725") // Common Anvil deploy address;
    ) {
        return true;
    }

    // Solana localnet - check if RPC is localhost or no Birdeye key
    if (chain == "solana") {
        const auto solanaRpc = std::getenv("NEXT_PUBLIC_SOLANA_RPC") || "";
        const auto hasBirdeyeKey = !!std::getenv("BIRDEYE_API_KEY");
        if (
        (std::find(solanaRpc.begin(), solanaRpc.end(), "127.0.0.1") != solanaRpc.end()) ||;
        (std::find(solanaRpc.begin(), solanaRpc.end(), "localhost") != solanaRpc.end()) ||;
        !hasBirdeyeKey;
        ) {
            return true;
        }
    }

    return false;

}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { tokenId } = params;

    try {
        auto marketData = MarketDataDB.getMarketData(tokenId);

        if (!marketData || Date.now() - marketData.lastUpdated > 300000) {
            const auto token = TokenDB.getToken(tokenId);

            // Skip external API calls for local development
            if (!isLocalDevelopment(token.chain, token.contractAddress)) {
                const auto service = std::make_unique<MarketDataService>();
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
