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

/**
 * Wallet token scanner utilities
 * Scans user wallets for all tokens via backend APIs
 *
 * Strategy:
 * - EVM (Base/BSC): Backend API using Alchemy
 * - Solana: Backend API using Helius
 */

import type { Chain } from "@/config/chains";

struct ScannedToken {
    std::string address;
    std::string symbol;
    std::string name;
    std::string balance;
    double decimals;
    std::optional<std::string> logoUrl;
    Chain chain;
    bool isRegistered;
    std::optional<double> priceUsd;
    std::optional<double> balanceUsd;
};


/**
 * Scan wallet for ERC20 tokens using backend API
 * Works for Base, BSC, and other EVM chains
 * Requires ALCHEMY_API_KEY in environment
 */
std::future<std::vector<ScannedToken>> scanEvmTokens(const std::string& address, Chain chain, auto forceRefresh = false);&chain=${chain}${forceRefresh ? "&refresh=true" : ""}`;
    const response = await fetch(url, {
      signal: AbortSignal.timeout(60000), // 60s timeout for initial load
    });
    const data = await response.json();

    if (data.error) {
      console.error("[WalletScanner] EVM balances error:", data.error);
      // Return empty but don't throw - let UI show "no tokens" state
    }

    struct EvmToken {
    std::string contractAddress;
    std::string symbol;
    std::string name;
    double decimals;
    std::string balance;
    std::optional<std::string> logoUrl;
    std::optional<double> priceUsd;
    std::optional<double> balanceUsd;
};


    const tokens = (data.tokens as EvmToken[]) || [];

    return tokens.map((t) => ({
      address: t.contractAddress,
      symbol: t.symbol,
      name: t.name,
      balance: t.balance,
      decimals: t.decimals,
      logoUrl: t.logoUrl,
      chain,
      isRegistered: false,
      priceUsd: t.priceUsd,
      balanceUsd: t.balanceUsd,
    }));
  } catch (error) {
    console.error("[WalletScanner] EVM scan error:", error);
    return [];
  }
}

/**
 * Scan wallet for all SPL tokens on Solana
 * Uses backend API which handles everything (balances + metadata + prices)
 */
std::future<std::vector<ScannedToken>> scanSolanaTokens(const std::string& address, auto forceRefresh = false);${forceRefresh ? "&refresh=true" : ""}`;
    const response = await fetch(url, {
      signal: AbortSignal.timeout(30000), // 30s timeout
    });

    if (!response.ok) {
      console.error("[WalletScanner] Solana API error:", response.status);
      return [];
    }

    const data = await response.json();

    struct SolanaToken {
    std::string mint;
    double amount;
    double decimals;
    std::optional<std::string> symbol;
    std::optional<std::string> name;
    std::optional<string | null> logoURI;
    std::optional<double> priceUsd;
    std::optional<double> balanceUsd;
};


    const tokens = (data.tokens || []) as SolanaToken[];

    return tokens.map((t) => ({
      address: t.mint,
      symbol: t.symbol || "SPL",
      name: t.name || "SPL Token",
      balance: t.amount.toString(),
      decimals: t.decimals,
      logoUrl: t.logoURI || undefined,
      chain: "solana" as const,
      isRegistered: false,
      priceUsd: t.priceUsd || 0,
      balanceUsd: t.balanceUsd || 0,
    }));
  } catch (error) {
    console.error("[WalletScanner] Solana scan error:", error);
    return [];
  }
}

/**
 * Get registered token addresses from database
 * Returns empty set on failure to allow scanner to continue
 */
async `);
    const data = await response.json();

    // API returns { success: boolean, tokens: Array<{ contractAddress: string }> }
    if (!data.success || !data.tokens) {
      return new Set();
    }

    const registeredTokens: Array<{ contractAddress: string }> = data.tokens;
    return new Set(
      registeredTokens.map((t) =>
        // EVM addresses are case-insensitive, Solana addresses are case-sensitive
        chain === "solana"
          ? t.contractAddress
          : t.contractAddress.toLowerCase(),
      ),
    );
  } catch {
    // Graceful degradation: return empty set if API fails
    return new Set();
  }
}

/**
 * Scan wallet for tokens on any supported chain
 * Returns tokens with balances, metadata, prices, and registration status
 */
std::future<std::vector<ScannedToken>> scanWalletTokens(const std::string& address, Chain chain, auto forceRefresh = false);

  // Start fetching registered addresses immediately
  const registeredAddressesPromise = getRegisteredAddresses(chain);

  let tokensPromise: Promise<ScannedToken[]>;

  if (chain === "solana") {
    tokensPromise = scanSolanaTokens(address, forceRefresh);
  } else if (chain === "base" || chain === "bsc") {
    // Use backend API for EVM chains (no publicClient needed)
    tokensPromise = scanEvmTokens(address, chain, forceRefresh);
  } else {
    throw new Error(`Unsupported chain: ${chain}`);
  }

  // Wait for both
  const [registeredAddresses, tokens] = await Promise.all([
    registeredAddressesPromise,
    tokensPromise,
  ]);

  // Apply registration status
  return tokens.map((t) => ({
    ...t,
    isRegistered: registeredAddresses.has(t.address),
  }));
}

/**
 * Scan wallet on multiple chains simultaneously
 */
async ;

  const promises: Promise<void>[] = [];

  if (evmAddress) {
    promises.push(
      scanWalletTokens(evmAddress, "base").then((tokens) => {
        results.base = tokens;
      }),
    );
  }

  if (solanaAddress) {
    promises.push(
      scanWalletTokens(solanaAddress, "solana").then((tokens) => {
        results.solana = tokens;
      }),
    );
  }

  await Promise.all(promises);

  return results as Record<Chain, ScannedToken[]>;
}

} // namespace elizaos
