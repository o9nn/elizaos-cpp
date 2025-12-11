import { useState, useEffect, useRef } from "react";
import { createPublicClient, http, erc20Abi } from "viem";
import { base } from "viem/chains";
import type { Token, TokenMarketData } from "@/services/database";

interface TokenCacheEntry {
  token: Token;
  marketData: TokenMarketData | null;
  fetchedAt: number;
}

const globalTokenCache = new Map<string, TokenCacheEntry>();
const pendingFetches = new Map<string, Promise<TokenCacheEntry | null>>();

const CACHE_DURATION = 30000; // 30 seconds

// Fallback: fetch token metadata from blockchain
async function fetchTokenFromChain(tokenId: string): Promise<Token | null> {
  const parts = tokenId.split("-");
  if (parts.length < 3) return null;

  const chain = parts[1];
  const address = parts[2] as `0x${string}`;

  if (!address?.startsWith("0x")) return null;

  // For now, only support base chain for on-chain fallback
  if (chain !== "base") return null;

  try {
    const publicClient = createPublicClient({
      chain: base,
      transport: http("/api/rpc/base"),
    });

    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    const readContract = publicClient.readContract.bind(publicClient) as any;
    const [symbol, name, decimals] = await Promise.all([
      readContract({
        address,
        abi: erc20Abi,
        functionName: "symbol",
      }),
      readContract({
        address,
        abi: erc20Abi,
        functionName: "name",
      }),
      readContract({
        address,
        abi: erc20Abi,
        functionName: "decimals",
      }),
    ]);

    return {
      id: tokenId,
      symbol: symbol as string,
      name: name as string,
      decimals: decimals as number,
      chain: chain as Token["chain"],
      contractAddress: address,
      logoUrl: "",
      description: "",
      isActive: true,
      createdAt: Date.now(),
      updatedAt: Date.now(),
    };
  } catch (err) {
    console.error("[useTokenCache] Failed to fetch token from chain:", err);
    return null;
  }
}

export function useTokenCache(tokenId: string | null) {
  const [token, setToken] = useState<Token | null>(null);
  const [marketData, setMarketData] = useState<TokenMarketData | null>(null);
  const [isLoading, setIsLoading] = useState(true);
  const fetchedTokenId = useRef<string | null>(null);

  useEffect(() => {
    if (!tokenId) {
      setToken(null);
      setMarketData(null);
      setIsLoading(false);
      return;
    }

    // If tokenId changed, reset state and fetch new token
    if (fetchedTokenId.current !== tokenId) {
      // Clear previous token data when tokenId changes
      if (fetchedTokenId.current !== null) {
        setToken(null);
        setMarketData(null);
        setIsLoading(true);
      }
      fetchedTokenId.current = tokenId;
    } else {
      // Already fetched this tokenId
      return;
    }

    async function loadToken(id: string) {
      // Check cache first (synchronously)
      const cached = globalTokenCache.get(id);
      const now = Date.now();

      if (cached && now - cached.fetchedAt < CACHE_DURATION) {
        setToken(cached.token);
        setMarketData(cached.marketData);
        setIsLoading(false);
        return;
      }

      // Check if fetch is already in progress (critical section)
      let fetchPromise = pendingFetches.get(id);

      if (!fetchPromise) {
        // Start new fetch - only ONE component will enter this block
        fetchPromise = (async () => {
          const response = await fetch(`/api/tokens/${id}`);
          const data = await response.json();

          if (data.success) {
            const entry: TokenCacheEntry = {
              token: data.token,
              marketData: data.marketData || null,
              fetchedAt: Date.now(),
            };
            globalTokenCache.set(id, entry);
            return entry;
          }

          // Fallback: fetch from blockchain if not in database
          const chainToken = await fetchTokenFromChain(id);
          if (chainToken) {
            const entry: TokenCacheEntry = {
              token: chainToken,
              marketData: null,
              fetchedAt: Date.now(),
            };
            globalTokenCache.set(id, entry);
            return entry;
          }

          return null;
        })();

        // Set pending IMMEDIATELY to block other components
        pendingFetches.set(id, fetchPromise);

        // Clean up pending fetch after it completes
        fetchPromise.finally(() => {
          pendingFetches.delete(id);
        });
      }

      // All components (first and subsequent) wait for the same promise
      const entry = await fetchPromise;

      if (entry) {
        setToken(entry.token);
        setMarketData(entry.marketData);
      }
      setIsLoading(false);
    }

    loadToken(tokenId);
  }, [tokenId]);

  return { token, marketData, isLoading };
}

const marketDataRefreshIntervals = new Map<
  string,
  ReturnType<typeof setInterval>
>();
const marketDataSubscribers = new Map<
  string,
  Set<(data: TokenMarketData) => void>
>();

function subscribeToMarketData(
  tokenId: string,
  callback: (data: TokenMarketData) => void,
) {
  if (!marketDataSubscribers.has(tokenId)) {
    marketDataSubscribers.set(tokenId, new Set());
  }
  marketDataSubscribers.get(tokenId)!.add(callback);

  // Set up refresh interval only once per tokenId
  if (!marketDataRefreshIntervals.has(tokenId)) {
    async function refreshMarketData() {
      const response = await fetch(`/api/market-data/${tokenId}`);

      if (!response.ok) {
        console.warn(
          `Failed to fetch market data for ${tokenId}:`,
          response.status,
        );
        return;
      }

      const data = await response.json();

      if (data.success && data.marketData) {
        // Update cache
        const cached = globalTokenCache.get(tokenId);
        if (cached) {
          cached.marketData = data.marketData;
        }

        // Notify all subscribers
        marketDataSubscribers
          .get(tokenId)
          ?.forEach((cb) => cb(data.marketData));
      }
    }

    const interval = setInterval(refreshMarketData, 30000); // 30 seconds
    marketDataRefreshIntervals.set(tokenId, interval);
  }

  return () => {
    const subscribers = marketDataSubscribers.get(tokenId);
    if (subscribers) {
      subscribers.delete(callback);
      // Clean up interval when no more subscribers
      if (subscribers.size === 0) {
        const interval = marketDataRefreshIntervals.get(tokenId);
        if (interval) {
          clearInterval(interval);
          marketDataRefreshIntervals.delete(tokenId);
        }
        marketDataSubscribers.delete(tokenId);
      }
    }
  };
}

export function useMarketDataRefresh(
  tokenId: string | null,
  token: Token | null,
) {
  const [marketData, setMarketData] = useState<TokenMarketData | null>(null);

  useEffect(() => {
    if (!token || !tokenId) return;

    return subscribeToMarketData(tokenId, setMarketData);
  }, [token, tokenId]);

  return marketData;
}
