#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

// Check if we're in local development mode (no external API calls needed)


  // Solana localnet - check if RPC is localhost or no Birdeye key
  if (chain === "solana") {
    const solanaRpc = process.env.NEXT_PUBLIC_SOLANA_RPC || "";
    const hasBirdeyeKey = !!process.env.BIRDEYE_API_KEY;
    if (
      solanaRpc.includes("127.0.0.1") ||
      solanaRpc.includes("localhost") ||
      !hasBirdeyeKey
    ) {
      return true;
    }
  }

  return false;
}

async : { params: Promise<{ tokenId: string }> },
) {
  const { tokenId } = await params;

  try {
    let marketData = await MarketDataDB.getMarketData(tokenId);

    if (!marketData || Date.now() - marketData.lastUpdated > 300000) {
      const token = await TokenDB.getToken(tokenId);

      // Skip external API calls for local development
      if (!isLocalDevelopment(token.chain, token.contractAddress)) {
        const service = new MarketDataService();
        await service.refreshTokenData(
          tokenId,
          token.contractAddress,
          token.chain,
        );
        marketData = await MarketDataDB.getMarketData(tokenId);
      }
    }

    return NextResponse.json({
      success: true,
      marketData,
    });
  } catch (error) {
    return NextResponse.json(
      {
        success: false,
        error:
          error instanceof Error
            ? error.message
            : "Failed to fetch market data",
      },
      { status: 404 },
    );
  }
}

} // namespace elizaos
