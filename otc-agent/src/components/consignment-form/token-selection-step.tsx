"use client";

import {
  useEffect,
  useState,
  useCallback,
  useReducer,
  useMemo,
  useRef,
} from "react";
import Image from "next/image";
import { useMultiWallet } from "../multiwallet";
import type { Token } from "@/services/database";
import { Button } from "../button";
import { useChainId } from "wagmi";
import { base, baseSepolia, bsc, bscTestnet } from "wagmi/chains";
import {
  scanWalletTokens,
  type ScannedToken,
} from "@/utils/wallet-token-scanner";
import type { Chain } from "@/config/chains";
import { usePrivy } from "@privy-io/react-auth";
import { Search, X, RefreshCw, ExternalLink, Loader2 } from "lucide-react";

// Address detection helpers
function isSolanaAddress(address: string): boolean {
  const base58Regex = /^[1-9A-HJ-NP-Za-km-z]{32,44}$/;
  return base58Regex.test(address);
}

function isEvmAddress(address: string): boolean {
  return /^0x[a-fA-F0-9]{40}$/i.test(address);
}

function isContractAddress(query: string): boolean {
  return isSolanaAddress(query) || isEvmAddress(query);
}

// Minimum thresholds to filter obvious dust
const MIN_TOKEN_BALANCE = 1; // At least 1 token (human-readable)
const MIN_VALUE_USD = 0.001; // $0.001 minimum if we have a price

// Client-side token cache (15 minute TTL)
const TOKEN_CACHE_TTL_MS = 15 * 60 * 1000;
interface CachedTokens {
  tokens: TokenWithBalance[];
  walletAddress: string;
  chain: string;
  cachedAt: number;
}

function getTokenCache(
  walletAddress: string,
  chain: string,
): TokenWithBalance[] | null {
  try {
    const cacheKey = `token-cache:${chain}:${walletAddress}`;
    const cached = localStorage.getItem(cacheKey);
    if (!cached) return null;

    const data: CachedTokens = JSON.parse(cached);
    // Check if cache is still valid (15 minutes)
    if (Date.now() - data.cachedAt >= TOKEN_CACHE_TTL_MS) {
      localStorage.removeItem(cacheKey);
      return null;
    }
    console.log(`[TokenSelection] Using cached tokens for ${chain}`);
    return data.tokens;
  } catch {
    return null;
  }
}

function setTokenCache(
  walletAddress: string,
  chain: string,
  tokens: TokenWithBalance[],
): void {
  try {
    const cacheKey = `token-cache:${chain}:${walletAddress}`;
    const data: CachedTokens = {
      tokens,
      walletAddress,
      chain,
      cachedAt: Date.now(),
    };
    localStorage.setItem(cacheKey, JSON.stringify(data));
  } catch {
    // Ignore storage errors
  }
}

function clearTokenCache(walletAddress?: string, chain?: string): void {
  try {
    if (walletAddress && chain) {
      localStorage.removeItem(`token-cache:${chain}:${walletAddress}`);
    } else {
      // Clear all token caches
      const keys = Object.keys(localStorage).filter((k) =>
        k.startsWith("token-cache:"),
      );
      keys.forEach((k) => localStorage.removeItem(k));
    }
  } catch {
    // Ignore
  }
}

export interface TokenWithBalance extends Token {
  balance: string;
  balanceUsd: number;
  priceUsd: number;
}

interface TokenSelectionProps {
  formData: { tokenId: string };
  updateFormData: (updates: { tokenId: string }) => void;
  onNext: () => void;
  onTokenSelect?: (token: TokenWithBalance) => void;
}

function formatBalance(balance: string, decimals: number): string {
  const num = Number(balance) / Math.pow(10, decimals);
  if (num >= 1_000_000) return `${(num / 1_000_000).toFixed(2)}M`;
  if (num >= 1_000) return `${(num / 1_000).toFixed(2)}K`;
  return num.toFixed(2);
}

function formatUsd(usd: number): string {
  if (usd >= 1_000_000) return `$${(usd / 1_000_000).toFixed(2)}M`;
  if (usd >= 1_000) return `$${(usd / 1_000).toFixed(2)}K`;
  return `$${usd.toFixed(2)}`;
}

interface LoadingState {
  isLoading: boolean;
  hasLoadedOnce: boolean;
}

type LoadingAction = { type: "START_LOADING" } | { type: "FINISH_LOADING" };

function loadingReducer(
  state: LoadingState,
  action: LoadingAction,
): LoadingState {
  switch (action.type) {
    case "START_LOADING":
      return { isLoading: true, hasLoadedOnce: false };
    case "FINISH_LOADING":
      return { isLoading: false, hasLoadedOnce: true };
    default:
      return state;
  }
}

export function TokenSelectionStep({
  formData,
  updateFormData,
  onNext,
  onTokenSelect,
}: TokenSelectionProps) {
  const {
    activeFamily,
    evmAddress,
    solanaPublicKey,
    hasWallet,
    privyAuthenticated,
    connectWallet,
  } = useMultiWallet();
  const { login, ready: privyReady } = usePrivy();
  const chainId = useChainId();
  const [tokens, setTokens] = useState<TokenWithBalance[]>([]);
  const [searchQuery, setSearchQuery] = useState("");
  const [loadingState, dispatchLoading] = useReducer(loadingReducer, {
    isLoading: true,
    hasLoadedOnce: false,
  });

  // State for address lookup
  const [searchedToken, setSearchedToken] = useState<TokenWithBalance | null>(
    null,
  );
  const [isSearchingAddress, setIsSearchingAddress] = useState(false);
  const [addressSearchError, setAddressSearchError] = useState<string | null>(
    null,
  );
  const addressSearchRef = useRef<string | null>(null);

  const { isLoading: loading, hasLoadedOnce } = loadingState;

  // Filter tokens by search query (symbol or name)
  const filteredTokens = useMemo(() => {
    if (!searchQuery.trim()) return tokens;
    const query = searchQuery.toLowerCase().trim();
    return tokens.filter(
      (t) =>
        t.symbol.toLowerCase().includes(query) ||
        t.name.toLowerCase().includes(query) ||
        t.contractAddress.toLowerCase().includes(query),
    );
  }, [tokens, searchQuery]);

  // Detect if we should search by address
  const searchIsAddress = useMemo(() => {
    const trimmed = searchQuery.trim();
    return trimmed.length > 0 && isContractAddress(trimmed);
  }, [searchQuery]);

  // Check if the searched address is already in wallet
  const addressFoundInWallet = useMemo(() => {
    if (!searchIsAddress) return false;
    const query = searchQuery.trim().toLowerCase();
    return tokens.some((t) => t.contractAddress.toLowerCase() === query);
  }, [searchIsAddress, searchQuery, tokens]);

  const handleConnect = useCallback(() => {
    if (privyAuthenticated) {
      connectWallet();
    } else {
      login();
    }
  }, [privyAuthenticated, connectWallet, login]);

  const getEvmChainName = useCallback((): Chain => {
    if (chainId === base.id || chainId === baseSepolia.id) return "base";
    if (chainId === bsc.id || chainId === bscTestnet.id) return "bsc";
    return "base";
  }, [chainId]);

  // Look up token by contract address when not found in wallet
  useEffect(() => {
    const trimmed = searchQuery.trim();

    // Clear if not a valid address or found in wallet
    if (!searchIsAddress || addressFoundInWallet) {
      setSearchedToken(null);
      setAddressSearchError(null);
      addressSearchRef.current = null;
      return;
    }

    // Don't re-search same address
    if (addressSearchRef.current === trimmed) return;

    // Detect chain from address format
    const chain: Chain = isSolanaAddress(trimmed)
      ? "solana"
      : activeFamily === "solana"
        ? "base"
        : getEvmChainName();

    // Debounce the lookup
    const timeoutId = setTimeout(async () => {
      addressSearchRef.current = trimmed;
      setIsSearchingAddress(true);
      setAddressSearchError(null);

      try {
        const response = await fetch(
          `/api/token-lookup?address=${encodeURIComponent(trimmed)}&chain=${chain}`,
        );
        const data = await response.json();

        if (data.success && data.token) {
          const token = data.token;
          setSearchedToken({
            id: `token-${token.chain}-${token.address}`,
            symbol: token.symbol,
            name: token.name,
            contractAddress: token.address,
            chain: token.chain as Chain,
            decimals: token.decimals,
            logoUrl: token.logoUrl || "",
            description: "",
            isActive: true,
            createdAt: Date.now(),
            updatedAt: Date.now(),
            balance: "0", // User doesn't have this token
            balanceUsd: 0,
            priceUsd: token.priceUsd || 0,
          });
          setAddressSearchError(null);
        } else {
          setSearchedToken(null);
          setAddressSearchError(data.error || "Token not found");
        }
      } catch (error) {
        console.error("[TokenSelection] Address lookup error:", error);
        setSearchedToken(null);
        setAddressSearchError("Failed to look up token");
      } finally {
        setIsSearchingAddress(false);
      }
    }, 500); // 500ms debounce

    return () => clearTimeout(timeoutId);
  }, [
    searchQuery,
    searchIsAddress,
    addressFoundInWallet,
    activeFamily,
    getEvmChainName,
  ]);

  // Track previous wallet to detect disconnects
  const prevWalletRef = useRef<string | null>(null);
  const [isRefreshing, setIsRefreshing] = useState(false);

  // Function to load tokens (can be called with forceRefresh)
  const loadUserTokens = useCallback(
    async (forceRefresh = false) => {
      const chain: Chain =
        activeFamily === "solana" ? "solana" : getEvmChainName();
      const userAddress =
        activeFamily === "solana" ? solanaPublicKey : evmAddress;

      if (!hasWallet || !userAddress) {
        setTokens([]);
        dispatchLoading({ type: "FINISH_LOADING" });
        return;
      }

      // Check client-side cache first (15 minute TTL) unless force refresh
      // Only use cache if it has tokens - empty results shouldn't be cached
      if (!forceRefresh) {
        const cachedTokens = getTokenCache(userAddress, chain);
        if (cachedTokens && cachedTokens.length > 0) {
          setTokens(cachedTokens);
          dispatchLoading({ type: "FINISH_LOADING" });
          return;
        }
      } else {
        // Clear client-side cache when force refreshing
        clearTokenCache(userAddress, chain);
      }

      dispatchLoading({ type: "START_LOADING" });
      if (forceRefresh) setIsRefreshing(true);

      try {
        // Fetch from backend APIs
        const scannedTokens: ScannedToken[] = await scanWalletTokens(
          userAddress,
          chain,
          forceRefresh,
        );

        // Build token list - prices already included from backend
        const tokensWithBalances: TokenWithBalance[] = scannedTokens
          .filter((t) => BigInt(t.balance) > 0n)
          .map((t) => ({
            id: `token-${t.chain}-${t.address}`,
            symbol: t.symbol,
            name: t.name,
            contractAddress: t.address,
            chain: t.chain,
            decimals: t.decimals,
            logoUrl: t.logoUrl || "",
            description: "",
            isActive: true,
            createdAt: Date.now(),
            updatedAt: Date.now(),
            balance: t.balance,
            balanceUsd: t.balanceUsd || 0,
            priceUsd: t.priceUsd || 0,
          }));

        // Apply minimal dust filter - show tokens without prices
        const filteredTokens = tokensWithBalances.filter((t) => {
          const humanBalance =
            Number(BigInt(t.balance)) / Math.pow(10, t.decimals);
          const hasPrice = t.priceUsd > 0;
          if (hasPrice && t.balanceUsd < MIN_VALUE_USD) {
            return false;
          }
          return humanBalance >= MIN_TOKEN_BALANCE;
        });

        // Sort: priced tokens first, then by balance
        filteredTokens.sort((a, b) => {
          const aHasPrice = a.priceUsd > 0;
          const bHasPrice = b.priceUsd > 0;
          if (aHasPrice && !bHasPrice) return -1;
          if (!aHasPrice && bHasPrice) return 1;
          if (aHasPrice && bHasPrice) return b.balanceUsd - a.balanceUsd;
          const aBalance = Number(BigInt(a.balance)) / Math.pow(10, a.decimals);
          const bBalance = Number(BigInt(b.balance)) / Math.pow(10, b.decimals);
          return bBalance - aBalance;
        });

        // Save to client-side cache (15 minute TTL) - only cache if we found tokens
        if (userAddress && filteredTokens.length > 0) {
          setTokenCache(userAddress, chain, filteredTokens);
        }
        setTokens(filteredTokens);
      } catch (error) {
        console.error("[TokenSelection] Scan error:", error);
      } finally {
        dispatchLoading({ type: "FINISH_LOADING" });
        setIsRefreshing(false);
      }
    },
    [activeFamily, evmAddress, solanaPublicKey, hasWallet, getEvmChainName],
  );

  // Auto-load on mount and when wallet changes
  useEffect(() => {
    const userAddress =
      activeFamily === "solana" ? solanaPublicKey : evmAddress;

    // Detect wallet change (disconnect/reconnect) - clear cache
    if (prevWalletRef.current && prevWalletRef.current !== userAddress) {
      console.log("[TokenSelection] Wallet changed, clearing cache");
      clearTokenCache();
    }
    prevWalletRef.current = userAddress || null;

    loadUserTokens();
  }, [loadUserTokens, activeFamily, evmAddress, solanaPublicKey]);

  // Refresh handler
  const handleRefresh = useCallback(() => {
    loadUserTokens(true);
  }, [loadUserTokens]);

  const handleTokenClick = (token: TokenWithBalance) => {
    updateFormData({ tokenId: token.id });
    onTokenSelect?.(token);
    onNext();
  };

  if (!hasWallet) {
    return (
      <div className="text-center py-8 space-y-4">
        <p className="text-zinc-600 dark:text-zinc-400">
          {privyAuthenticated
            ? "Connect a wallet to list your tokens"
            : "Sign in to list your tokens"}
        </p>
        <Button
          color="brand"
          onClick={handleConnect}
          disabled={!privyReady}
          className="!px-8 !py-3"
        >
          {privyReady
            ? privyAuthenticated
              ? "Connect Wallet"
              : "Sign In"
            : "Loading..."}
        </Button>
      </div>
    );
  }

  if (loading) {
    return (
      <div className="text-center py-8">
        <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-brand-500 mx-auto mb-4" />
        <p className="text-zinc-600 dark:text-zinc-400">
          Loading your tokens...
        </p>
      </div>
    );
  }

  if (tokens.length === 0 && hasLoadedOnce) {
    return (
      <div className="text-center py-8 space-y-4">
        <p className="text-zinc-600 dark:text-zinc-400">
          No {activeFamily === "solana" ? "Solana" : "EVM"} tokens found in your
          wallet.
        </p>
        <p className="text-sm text-zinc-500 dark:text-zinc-500">
          {activeFamily === "solana"
            ? "Make sure you have tokens on Solana mainnet."
            : "Make sure you have tokens on Base/BSC and ALCHEMY_API_KEY is configured."}
        </p>
        <p className="text-xs text-zinc-400 dark:text-zinc-500">
          Or use the network selector above to switch chains.
        </p>
      </div>
    );
  }

  return (
    <div className="flex flex-col h-full min-h-0 space-y-3">
      <div className="flex items-center justify-between">
        <p className="text-sm text-zinc-600 dark:text-zinc-400">
          Select a token to list for OTC trading ({tokens.length} tokens)
        </p>
        <button
          onClick={handleRefresh}
          disabled={isRefreshing || loading}
          className="flex items-center gap-1.5 px-2.5 py-1.5 text-xs text-zinc-500 hover:text-zinc-700 dark:text-zinc-400 dark:hover:text-zinc-200 hover:bg-zinc-100 dark:hover:bg-zinc-800 rounded-lg transition-colors disabled:opacity-50"
          title="Refresh token list"
        >
          <RefreshCw
            className={`w-3.5 h-3.5 ${isRefreshing ? "animate-spin" : ""}`}
          />
          {isRefreshing ? "Refreshing..." : "Refresh"}
        </button>
      </div>

      {/* Search input */}
      <div className="relative">
        <Search className="absolute left-3 top-1/2 -translate-y-1/2 w-4 h-4 text-zinc-400" />
        <input
          type="text"
          placeholder="Search by name, symbol, or address..."
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          className="w-full pl-10 pr-10 py-2.5 rounded-lg border border-zinc-200 dark:border-zinc-700 bg-white dark:bg-zinc-800/50 text-sm focus:ring-2 focus:ring-brand-500/20 focus:border-brand-500 transition-all"
        />
        {searchQuery && (
          <button
            onClick={() => setSearchQuery("")}
            className="absolute right-3 top-1/2 -translate-y-1/2 text-zinc-400 hover:text-zinc-600"
          >
            <X className="w-4 h-4" />
          </button>
        )}
      </div>

      {/* Show searched token from address lookup */}
      {searchIsAddress && !addressFoundInWallet && (
        <div className="mb-3">
          {isSearchingAddress ? (
            <div className="flex items-center justify-center gap-2 py-4 text-zinc-500">
              <Loader2 className="w-4 h-4 animate-spin" />
              <span className="text-sm">Looking up token...</span>
            </div>
          ) : searchedToken ? (
            <div className="space-y-2">
              <p className="text-xs text-zinc-500 flex items-center gap-1">
                <ExternalLink className="w-3 h-3" />
                Token found by address (not in your wallet)
              </p>
              <div
                onClick={() => handleTokenClick(searchedToken)}
                className={`p-4 rounded-xl border cursor-pointer transition-all hover:scale-[1.01] hover:shadow-md border-brand-300 dark:border-brand-700 bg-brand-500/5 ${
                  formData.tokenId === searchedToken.id
                    ? "ring-2 ring-brand-500/20"
                    : ""
                }`}
              >
                <div className="flex items-center gap-3">
                  {searchedToken.logoUrl ? (
                    <Image
                      src={searchedToken.logoUrl}
                      alt={searchedToken.symbol}
                      width={44}
                      height={44}
                      className="w-11 h-11 rounded-full ring-2 ring-brand-200 dark:ring-brand-800"
                    />
                  ) : (
                    <div className="w-11 h-11 rounded-full bg-gradient-to-br from-brand-400 to-brand-500 flex items-center justify-center">
                      <span className="text-white font-bold text-lg">
                        {searchedToken.symbol.charAt(0)}
                      </span>
                    </div>
                  )}
                  <div className="flex-1 min-w-0">
                    <div className="flex items-center justify-between">
                      <div className="font-semibold text-zinc-900 dark:text-zinc-100">
                        {searchedToken.symbol}
                      </div>
                      <span className="text-xs bg-brand-100 dark:bg-brand-900/30 text-brand-700 dark:text-brand-300 px-2 py-0.5 rounded-full">
                        {searchedToken.chain}
                      </span>
                    </div>
                    <div className="text-sm text-zinc-600 dark:text-zinc-400 truncate pr-2">
                      {searchedToken.name}
                    </div>
                    <div className="text-xs text-zinc-400 dark:text-zinc-500 font-mono mt-1">
                      {searchedToken.contractAddress.slice(0, 8)}...
                      {searchedToken.contractAddress.slice(-6)}
                    </div>
                  </div>
                  <div className="ml-2 text-brand-500">
                    <svg
                      className="w-5 h-5"
                      fill="none"
                      stroke="currentColor"
                      viewBox="0 0 24 24"
                    >
                      <path
                        strokeLinecap="round"
                        strokeLinejoin="round"
                        strokeWidth={2}
                        d="M9 5l7 7-7 7"
                      />
                    </svg>
                  </div>
                </div>
              </div>
            </div>
          ) : addressSearchError ? (
            <p className="text-sm text-brand-600 dark:text-brand-400 text-center py-4">
              {addressSearchError === "Token not found"
                ? `No token found at ${searchQuery.slice(0, 8)}...${searchQuery.slice(-4)}`
                : addressSearchError}
            </p>
          ) : null}
        </div>
      )}

      {filteredTokens.length === 0 && searchQuery && !searchIsAddress && (
        <p className="text-sm text-zinc-500 text-center py-4">
          No tokens found matching &quot;{searchQuery}&quot;
        </p>
      )}

      {filteredTokens.length === 0 &&
        searchQuery &&
        searchIsAddress &&
        addressFoundInWallet && (
          <p className="text-sm text-zinc-500 text-center py-4">
            Token found in your wallet
          </p>
        )}

      {/* Divider when showing both searched token and wallet tokens */}
      {searchedToken && !addressFoundInWallet && filteredTokens.length > 0 && (
        <div className="flex items-center gap-3 py-1">
          <div className="flex-1 h-px bg-zinc-200 dark:bg-zinc-700" />
          <span className="text-xs text-zinc-400">Your wallet tokens</span>
          <div className="flex-1 h-px bg-zinc-200 dark:bg-zinc-700" />
        </div>
      )}

      <div className="flex-1 min-h-0 max-h-[45dvh] sm:max-h-[55dvh] overflow-y-auto space-y-3 pr-1 -mr-1">
        {filteredTokens.map((token) => (
          <div
            key={token.id}
            onClick={() => handleTokenClick(token)}
            className={`p-4 rounded-xl border cursor-pointer transition-all hover:scale-[1.01] hover:shadow-md ${
              formData.tokenId === token.id
                ? "border-brand-500 bg-brand-500/5 ring-2 ring-brand-500/20"
                : "border-zinc-200 dark:border-zinc-700 hover:border-brand-300 dark:hover:border-brand-700"
            }`}
          >
            <div className="flex items-center gap-3">
              {token.logoUrl ? (
                <Image
                  src={token.logoUrl}
                  alt={token.symbol}
                  width={44}
                  height={44}
                  className="w-11 h-11 rounded-full ring-2 ring-zinc-100 dark:ring-zinc-800"
                />
              ) : (
                <div className="w-11 h-11 rounded-full bg-gradient-to-br from-brand-400 to-brand-500 flex items-center justify-center">
                  <span className="text-white font-bold text-lg">
                    {token.symbol.charAt(0)}
                  </span>
                </div>
              )}
              <div className="flex-1 min-w-0">
                <div className="flex items-center justify-between">
                  <div className="font-semibold text-zinc-900 dark:text-zinc-100">
                    {token.symbol}
                  </div>
                  <div className="text-sm font-medium text-zinc-900 dark:text-zinc-100">
                    {formatUsd(token.balanceUsd)}
                  </div>
                </div>
                <div className="flex items-center justify-between">
                  <div className="text-sm text-zinc-600 dark:text-zinc-400 truncate pr-2">
                    {token.name}
                  </div>
                  <div className="text-xs text-zinc-500 dark:text-zinc-500 whitespace-nowrap">
                    {formatBalance(token.balance, token.decimals)}{" "}
                    {token.symbol}
                  </div>
                </div>
                <div className="text-xs text-zinc-400 dark:text-zinc-500 font-mono mt-1">
                  {token.contractAddress.slice(0, 6)}...
                  {token.contractAddress.slice(-4)}
                </div>
              </div>
              <div className="ml-2 text-brand-500">
                <svg
                  className="w-5 h-5"
                  fill="none"
                  stroke="currentColor"
                  viewBox="0 0 24 24"
                >
                  <path
                    strokeLinecap="round"
                    strokeLinejoin="round"
                    strokeWidth={2}
                    d="M9 5l7 7-7 7"
                  />
                </svg>
              </div>
            </div>
          </div>
        ))}
      </div>
    </div>
  );
}
