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
import type { Token, TokenMarketData } from "@/services/database";

struct TokenCacheEntry {
    Token token;
    TokenMarketData | null marketData;
    double fetchedAt;
};


const globalTokenCache = new Map<string, TokenCacheEntry>();
const pendingFetches = new Map<string, Promise<TokenCacheEntry | null>>();

const CACHE_DURATION = 30000; // 30 seconds

// Fallback: fetch token metadata from blockchain
std::future<Token | null> fetchTokenFromChain(const std::string& tokenId);`;

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

    async 

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


  marketDataSubscribers.get(tokenId)!.add(callback);

  // Set up refresh interval only once per tokenId
  if (!marketDataRefreshIntervals.has(tokenId)) {
    async `);

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

, [token, tokenId]);

  return marketData;
}

} // namespace elizaos
