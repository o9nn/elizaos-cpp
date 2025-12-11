import { NextRequest, NextResponse } from "next/server";
import { agentRuntime } from "@/lib/agent-runtime";

interface CachedPrice {
  priceUsd: number;
  cachedAt: number;
}

// Price cache TTL: 5 minutes
const PRICE_CACHE_TTL_MS = 5 * 60 * 1000;

/**
 * Get cached token price
 */
async function getCachedPrice(
  chain: string,
  address: string,
): Promise<number | null> {
  try {
    const runtime = await agentRuntime.getRuntime();
    const cacheKey = `token-price:${chain}:${address.toLowerCase()}`;
    const cached = await runtime.getCache<CachedPrice>(cacheKey);

    if (!cached) return null;
    if (Date.now() - cached.cachedAt >= PRICE_CACHE_TTL_MS) return null;

    return cached.priceUsd;
  } catch {
    return null;
  }
}

/**
 * Set cached token price
 */
async function setCachedPrice(
  chain: string,
  address: string,
  priceUsd: number,
): Promise<void> {
  try {
    const runtime = await agentRuntime.getRuntime();
    const cacheKey = `token-price:${chain}:${address.toLowerCase()}`;
    await runtime.setCache(cacheKey, {
      priceUsd,
      cachedAt: Date.now(),
    });
  } catch {
    // Ignore cache errors
  }
}

/**
 * Fetch Solana token prices from Jupiter Price API
 * Returns prices for multiple tokens in one call
 */
async function fetchSolanaPrices(
  mints: string[],
): Promise<Record<string, number>> {
  if (mints.length === 0) return {};

  try {
    // Jupiter supports up to 100 tokens per request
    const chunks: string[][] = [];
    for (let i = 0; i < mints.length; i += 100) {
      chunks.push(mints.slice(i, i + 100));
    }

    const allPrices: Record<string, number> = {};

    for (const chunk of chunks) {
      const ids = chunk.join(",");
      const response = await fetch(`https://api.jup.ag/price/v2?ids=${ids}`, {
        signal: AbortSignal.timeout(10000),
      });

      if (!response.ok) continue;

      const data = await response.json();

      // Jupiter returns { data: { [mint]: { price: string } } }
      if (data.data) {
        for (const [mint, priceData] of Object.entries(data.data)) {
          const price = (priceData as { price?: string })?.price;
          if (price) {
            allPrices[mint] = parseFloat(price);
          }
        }
      }
    }

    return allPrices;
  } catch (error) {
    console.error("[Token Prices] Jupiter API error:", error);
    return {};
  }
}

/**
 * CoinGecko platform IDs
 */
const COINGECKO_PLATFORMS: Record<string, string> = {
  base: "base",
  bsc: "binance-smart-chain",
  ethereum: "ethereum",
};

/**
 * Fetch EVM token prices from CoinGecko
 */
async function fetchEvmPrices(
  chain: string,
  addresses: string[],
): Promise<Record<string, number>> {
  if (addresses.length === 0) return {};

  const platformId = COINGECKO_PLATFORMS[chain];
  if (!platformId) return {};

  try {
    // CoinGecko supports multiple addresses in one call
    const addressList = addresses.map((a) => a.toLowerCase()).join(",");
    const apiKey = process.env.COINGECKO_API_KEY;

    const url = apiKey
      ? `https://pro-api.coingecko.com/api/v3/simple/token_price/${platformId}?contract_addresses=${addressList}&vs_currencies=usd`
      : `https://api.coingecko.com/api/v3/simple/token_price/${platformId}?contract_addresses=${addressList}&vs_currencies=usd`;

    const headers: HeadersInit = { "User-Agent": "OTC-Desk/1.0" };
    if (apiKey) {
      headers["X-Cg-Pro-Api-Key"] = apiKey;
    }

    const response = await fetch(url, {
      headers,
      signal: AbortSignal.timeout(10000),
    });

    if (!response.ok) {
      console.error("[Token Prices] CoinGecko API error:", response.status);
      return {};
    }

    const data = await response.json();
    const prices: Record<string, number> = {};

    // CoinGecko returns { [address]: { usd: number } }
    for (const [address, priceData] of Object.entries(data)) {
      const usd = (priceData as { usd?: number })?.usd;
      if (typeof usd === "number") {
        prices[address.toLowerCase()] = usd;
      }
    }

    return prices;
  } catch (error) {
    console.error("[Token Prices] CoinGecko API error:", error);
    return {};
  }
}

/**
 * GET /api/token-prices?chain=solana&addresses=mint1,mint2
 * Returns cached prices with 5-minute TTL
 */
export async function GET(request: NextRequest) {
  const chain = request.nextUrl.searchParams.get("chain");
  const addressesParam = request.nextUrl.searchParams.get("addresses");

  if (!chain || !addressesParam) {
    return NextResponse.json(
      { error: "chain and addresses required" },
      { status: 400 },
    );
  }

  const addresses = addressesParam.split(",").filter(Boolean);
  if (addresses.length === 0) {
    return NextResponse.json({ prices: {} });
  }

  // Check cache for each address
  const prices: Record<string, number> = {};
  const uncachedAddresses: string[] = [];

  for (const addr of addresses) {
    const cached = await getCachedPrice(chain, addr);
    if (cached !== null) {
      prices[addr] = cached;
    } else {
      uncachedAddresses.push(addr);
    }
  }

  // Fetch uncached prices
  if (uncachedAddresses.length > 0) {
    let freshPrices: Record<string, number> = {};

    if (chain === "solana") {
      freshPrices = await fetchSolanaPrices(uncachedAddresses);
    } else {
      freshPrices = await fetchEvmPrices(chain, uncachedAddresses);
    }

    // Cache and merge fresh prices
    for (const [addr, price] of Object.entries(freshPrices)) {
      await setCachedPrice(chain, addr, price);
      // Match original case for Solana
      const originalAddr =
        uncachedAddresses.find((a) => a.toLowerCase() === addr.toLowerCase()) ||
        addr;
      prices[originalAddr] = price;
    }
  }

  return NextResponse.json({ prices });
}
