import { NextRequest, NextResponse } from "next/server";
import { MarketDataDB, TokenDB, type Chain } from "@/services/database";
import { TokenRegistryService } from "@/services/tokenRegistry";
import { MarketDataService } from "@/services/marketDataService";
import { agentRuntime } from "@/lib/agent-runtime";

export async function GET(request: NextRequest) {
  const { searchParams } = new URL(request.url);
  const chain = searchParams.get("chain") as Chain | null;
  const minMarketCap = searchParams.get("minMarketCap");
  const maxMarketCap = searchParams.get("maxMarketCap");
  const isActive = searchParams.get("isActive");

  const service = new TokenRegistryService();

  const filters: Parameters<typeof service.getAllTokens>[0] = {};
  if (chain) filters.chain = chain;
  if (isActive !== null) filters.isActive = isActive === "true";
  if (minMarketCap) filters.minMarketCap = Number(minMarketCap);
  if (maxMarketCap) filters.maxMarketCap = Number(maxMarketCap);

  const tokens = await service.getAllTokens(filters);

  const tokensWithMarketData = await Promise.all(
    tokens.map(async (token) => {
      const marketData = await MarketDataDB.getMarketData(token.id);
      return {
        ...token,
        marketData,
      };
    }),
  );

  // Cache for 5 minutes - token metadata rarely changes
  return NextResponse.json(
    { success: true, tokens: tokensWithMarketData },
    {
      headers: {
        "Cache-Control": "public, s-maxage=300, stale-while-revalidate=600",
      },
    },
  );
}

export async function POST(request: NextRequest) {
  const body = await request.json();
  const {
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
    !symbol ||
    !name ||
    !contractAddress ||
    !chain ||
    decimals === undefined
  ) {
    return NextResponse.json(
      { error: "Missing required fields" },
      { status: 400 },
    );
  }

  const service = new TokenRegistryService();
  const token = await service.registerToken({
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

  const isLocalTestnet =
    contractAddress.startsWith("0x5FbDB") ||
    contractAddress.startsWith("0x5fbdb") ||
    (chain === "ethereum" && contractAddress.length === 42);

  const isSolanaWithoutKey = chain === "solana" && !process.env.BIRDEYE_API_KEY;

  if (!isLocalTestnet && !isSolanaWithoutKey) {
    const marketDataService = new MarketDataService();
    await marketDataService.refreshTokenData(token.id, contractAddress, chain);
  } else {
    await MarketDataDB.setMarketData({
      tokenId: token.id,
      priceUsd: 0.05,
      marketCap: 5000000,
      volume24h: 500000,
      priceChange24h: 0,
      liquidity: 1000000,
      lastUpdated: Date.now(),
    });
  }

  return NextResponse.json({
    success: true,
    token,
  });
}

/**
 * PATCH /api/tokens - Update token metadata
 * Body: { tokenId, updates: { logoUrl?, name?, symbol?, ... } }
 */
export async function PATCH(request: NextRequest) {
  const body = await request.json();
  const { tokenId, updates } = body;

  if (!tokenId || !updates) {
    return NextResponse.json(
      { error: "tokenId and updates are required" },
      { status: 400 },
    );
  }

  try {
    const updated = await TokenDB.updateToken(tokenId, updates);
    return NextResponse.json({ success: true, token: updated });
  } catch (error) {
    return NextResponse.json(
      { error: error instanceof Error ? error.message : "Update failed" },
      { status: 404 },
    );
  }
}

/**
 * DELETE /api/tokens - Clear all test/seeded tokens
 * Use with ?confirm=true to actually delete
 * Use with ?tokenId=xxx to delete a specific token
 */
export async function DELETE(request: NextRequest) {
  const { searchParams } = new URL(request.url);
  const confirm = searchParams.get("confirm") === "true";
  const tokenId = searchParams.get("tokenId");

  if (!confirm) {
    return NextResponse.json(
      { error: "Add ?confirm=true to actually delete tokens" },
      { status: 400 },
    );
  }

  const runtime = await agentRuntime.getRuntime();

  if (tokenId) {
    // Delete a specific token
    const token = await runtime.getCache(`token:${tokenId}`);
    if (!token) {
      return NextResponse.json({ error: "Token not found" }, { status: 404 });
    }

    await runtime.deleteCache(`token:${tokenId}`);
    await runtime.deleteCache(`market_data:${tokenId}`);

    // Remove from all_tokens index
    const allTokens = (await runtime.getCache<string[]>("all_tokens")) ?? [];
    const updated = allTokens.filter((id) => id !== tokenId);
    await runtime.setCache("all_tokens", updated);

    return NextResponse.json({
      success: true,
      message: `Deleted token: ${tokenId}`,
    });
  }

  // Delete ALL tokens
  const allTokens = (await runtime.getCache<string[]>("all_tokens")) ?? [];
  const deleted: string[] = [];

  for (const id of allTokens) {
    await runtime.deleteCache(`token:${id}`);
    await runtime.deleteCache(`market_data:${id}`);
    deleted.push(id);
  }

  // Clear the index
  await runtime.setCache("all_tokens", []);

  // Also clear consignments since they reference tokens
  const allConsignments =
    (await runtime.getCache<string[]>("all_consignments")) ?? [];
  for (const id of allConsignments) {
    await runtime.deleteCache(`consignment:${id}`);
  }
  await runtime.setCache("all_consignments", []);

  return NextResponse.json({
    success: true,
    message: `Deleted ${deleted.length} tokens and all consignments`,
    deletedTokens: deleted,
  });
}
