/**
 * Wallet token scanner utilities
 * Scans user wallets for all tokens via backend APIs
 *
 * Strategy:
 * - EVM (Base/BSC): Backend API using Alchemy
 * - Solana: Backend API using Helius
 */

import type { Chain } from "@/config/chains";

export interface ScannedToken {
  address: string;
  symbol: string;
  name: string;
  balance: string;
  decimals: number;
  logoUrl?: string;
  chain: Chain;
  isRegistered: boolean;
  priceUsd?: number;
  balanceUsd?: number;
}

/**
 * Scan wallet for ERC20 tokens using backend API
 * Works for Base, BSC, and other EVM chains
 * Requires ALCHEMY_API_KEY in environment
 */
async function scanEvmTokens(
  address: string,
  chain: Chain,
  forceRefresh = false,
): Promise<ScannedToken[]> {
  try {
    const url = `/api/evm-balances?address=${address}&chain=${chain}${forceRefresh ? "&refresh=true" : ""}`;
    const response = await fetch(url, {
      signal: AbortSignal.timeout(60000), // 60s timeout for initial load
    });
    const data = await response.json();

    if (data.error) {
      console.error("[WalletScanner] EVM balances error:", data.error);
      // Return empty but don't throw - let UI show "no tokens" state
    }

    interface EvmToken {
      contractAddress: string;
      symbol: string;
      name: string;
      decimals: number;
      balance: string;
      logoUrl?: string;
      priceUsd?: number;
      balanceUsd?: number;
    }

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
async function scanSolanaTokens(
  address: string,
  forceRefresh = false,
): Promise<ScannedToken[]> {
  try {
    // Backend API does everything: balances, metadata, prices in optimized calls
    const url = `/api/solana-balances?address=${address}${forceRefresh ? "&refresh=true" : ""}`;
    const response = await fetch(url, {
      signal: AbortSignal.timeout(30000), // 30s timeout
    });

    if (!response.ok) {
      console.error("[WalletScanner] Solana API error:", response.status);
      return [];
    }

    const data = await response.json();

    interface SolanaToken {
      mint: string;
      amount: number;
      decimals: number;
      symbol?: string;
      name?: string;
      logoURI?: string | null;
      priceUsd?: number;
      balanceUsd?: number;
    }

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
async function getRegisteredAddresses(chain: Chain): Promise<Set<string>> {
  try {
    const response = await fetch(`/api/tokens?chain=${chain}`);
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
export async function scanWalletTokens(
  address: string,
  chain: Chain,
  forceRefresh = false,
): Promise<ScannedToken[]> {
  if (!address) {
    throw new Error("Wallet address required");
  }

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
export async function scanWalletMultiChain(
  evmAddress?: string,
  solanaAddress?: string,
): Promise<Record<Chain, ScannedToken[]>> {
  const results: Record<string, ScannedToken[]> = {};

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
