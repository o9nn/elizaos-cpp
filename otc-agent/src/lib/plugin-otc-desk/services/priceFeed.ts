// Price feed service for fetching real-time token prices
// For multi-token support, use MarketDataService

interface PriceCache {
  price: number;
  timestamp: number;
}

const CACHE_TTL = 60 * 1000; // 60 seconds

/**
 * Get cached price from runtime storage
 */
async function getCachedPrice(key: string): Promise<PriceCache | null> {
  const { agentRuntime } = await import("../../agent-runtime");
  const runtime = await agentRuntime.getRuntime();
  return (await runtime.getCache<PriceCache>(`price:${key}`)) ?? null;
}

/**
 * Set cached price in runtime storage
 */
async function setCachedPrice(key: string, value: PriceCache): Promise<void> {
  const { agentRuntime } = await import("../../agent-runtime");
  const runtime = await agentRuntime.getRuntime();
  await runtime.setCache(`price:${key}`, value);
}

/**
 * Get ETH price in USD
 */
export async function getEthPriceUsd(): Promise<number> {
  const cacheKey = "ETH";

  // Check runtime cache
  const cached = await getCachedPrice(cacheKey);
  if (cached && Date.now() - cached.timestamp < CACHE_TTL) {
    return cached.price;
  }

  // Fetch from CoinGecko
  const response = await fetch(
    "https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd",
    {
      headers: {
        Accept: "application/json",
      },
    },
  );

  if (response.ok) {
    const data = await response.json();
    const price = data.ethereum?.usd;

    if (typeof price === "number") {
      await setCachedPrice(cacheKey, {
        price,
        timestamp: Date.now(),
      });
      return price;
    }
  }

  throw new Error("Failed to fetch ETH price");
}

/**
 * Get SOL price in USD
 */
export async function getSolPriceUsd(): Promise<number> {
  const cacheKey = "SOL";

  // Check runtime cache
  const cached = await getCachedPrice(cacheKey);
  if (cached && Date.now() - cached.timestamp < CACHE_TTL) {
    return cached.price;
  }

  // Fetch from CoinGecko
  const response = await fetch(
    "https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd",
    {
      headers: {
        Accept: "application/json",
      },
    },
  );

  if (response.ok) {
    const data = await response.json();
    const price = data.solana?.usd;

    if (typeof price === "number") {
      await setCachedPrice(cacheKey, {
        price,
        timestamp: Date.now(),
      });
      return price;
    }
  }

  throw new Error("Failed to fetch SOL price");
}

/**
 * Format token amount with proper display (K, M, B suffixes)
 */
export function formatTokenAmount(amount: string | number): string {
  const num = typeof amount === "string" ? parseFloat(amount) : amount;

  if (isNaN(num)) return "0";

  // Format with appropriate decimal places based on token value
  if (num >= 1000000000) {
    return `${(num / 1000000000).toFixed(2)}B`;
  } else if (num >= 1000000) {
    return `${(num / 1000000).toFixed(2)}M`;
  } else if (num >= 1000) {
    return `${(num / 1000).toFixed(2)}K`;
  } else {
    return num.toLocaleString();
  }
}
