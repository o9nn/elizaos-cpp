import { NextRequest, NextResponse } from "next/server";
import { agentRuntime } from "@/lib/agent-runtime";
import { put, head } from "@vercel/blob";
import crypto from "crypto";

// Wallet balance cache TTL: 15 minutes
const WALLET_CACHE_TTL_MS = 15 * 60 * 1000;

// Price cache TTL: 15 minutes
const PRICE_CACHE_TTL_MS = 15 * 60 * 1000;

// Codex GraphQL endpoint and Solana network ID
const CODEX_GRAPHQL_URL = "https://graph.codex.io/graphql";
const SOLANA_NETWORK_ID = 1399811149;

// Bulk metadata cache for Solana tokens (permanent - metadata doesn't change)
interface SolanaMetadataCache {
  metadata: Record<
    string,
    { symbol: string; name: string; logoURI: string | null }
  >;
}

async function getSolanaMetadataCache(): Promise<
  Record<string, { symbol: string; name: string; logoURI: string | null }>
> {
  try {
    const runtime = await agentRuntime.getRuntime();
    const cached = await runtime.getCache<SolanaMetadataCache>(
      "solana-metadata-bulk",
    );
    return cached?.metadata || {};
  } catch {
    return {};
  }
}

async function setSolanaMetadataCache(
  metadata: Record<
    string,
    { symbol: string; name: string; logoURI: string | null }
  >,
): Promise<void> {
  try {
    const runtime = await agentRuntime.getRuntime();
    await runtime.setCache("solana-metadata-bulk", { metadata });
  } catch {
    // Ignore
  }
}

// Bulk price cache for Solana
interface SolanaPriceCache {
  prices: Record<string, number>;
  cachedAt: number;
}

async function getSolanaPriceCache(): Promise<Record<string, number>> {
  try {
    const runtime = await agentRuntime.getRuntime();
    const cached =
      await runtime.getCache<SolanaPriceCache>("solana-prices-bulk");
    if (!cached) return {};
    if (Date.now() - cached.cachedAt >= PRICE_CACHE_TTL_MS) return {};
    console.log(
      `[Solana Balances] Using cached prices (${Object.keys(cached.prices).length} tokens)`,
    );
    return cached.prices;
  } catch {
    return {};
  }
}

async function setSolanaPriceCache(
  prices: Record<string, number>,
): Promise<void> {
  try {
    const runtime = await agentRuntime.getRuntime();
    await runtime.setCache("solana-prices-bulk", {
      prices,
      cachedAt: Date.now(),
    });
  } catch {
    // Ignore
  }
}

interface CachedWalletResponse {
  tokens: Array<{
    mint: string;
    amount: number;
    decimals: number;
    symbol: string;
    name: string;
    logoURI: string | null;
    priceUsd: number;
    balanceUsd: number;
  }>;
  cachedAt: number;
}

async function getCachedWalletResponse(
  address: string,
): Promise<CachedWalletResponse["tokens"] | null> {
  try {
    const runtime = await agentRuntime.getRuntime();
    const cached = await runtime.getCache<CachedWalletResponse>(
      `solana-wallet:${address}`,
    );
    if (!cached) return null;
    if (Date.now() - cached.cachedAt >= WALLET_CACHE_TTL_MS) return null;
    console.log(
      `[Solana Balances] Using cached wallet data (${cached.tokens.length} tokens)`,
    );
    return cached.tokens;
  } catch {
    return null;
  }
}

async function setCachedWalletResponse(
  address: string,
  tokens: CachedWalletResponse["tokens"],
): Promise<void> {
  try {
    const runtime = await agentRuntime.getRuntime();
    await runtime.setCache(`solana-wallet:${address}`, {
      tokens,
      cachedAt: Date.now(),
    });
  } catch {
    // Ignore
  }
}

// Alternative IPFS gateways to try if main one fails
const IPFS_GATEWAYS = [
  "https://cloudflare-ipfs.com",
  "https://dweb.link",
  "https://gateway.pinata.cloud",
  "https://ipfs.io",
];

/**
 * Try to fetch image from multiple IPFS gateways
 */
async function fetchWithIpfsGatewayFallback(
  imageUrl: string,
): Promise<Response | null> {
  // Extract IPFS hash from various URL formats
  let ipfsHash: string | null = null;

  // Match various IPFS URL patterns
  const patterns = [
    /ipfs\.io\/ipfs\/([a-zA-Z0-9]+)/,
    /\.mypinata\.cloud\/ipfs\/([a-zA-Z0-9]+)/,
    /cloudflare-ipfs\.com\/ipfs\/([a-zA-Z0-9]+)/,
    /dweb\.link\/ipfs\/([a-zA-Z0-9]+)/,
    /gateway\.pinata\.cloud\/ipfs\/([a-zA-Z0-9]+)/,
  ];

  for (const pattern of patterns) {
    const match = imageUrl.match(pattern);
    if (match) {
      ipfsHash = match[1];
      break;
    }
  }

  // If it's an IPFS URL, try multiple gateways
  if (ipfsHash) {
    const ipfsPath = `/ipfs/${ipfsHash}`;

    for (const gateway of IPFS_GATEWAYS) {
      try {
        const gatewayUrl = `${gateway}${ipfsPath}`;
        const response = await fetch(gatewayUrl, {
          headers: { "User-Agent": "OTC-Desk/1.0" },
          signal: AbortSignal.timeout(8000), // 8s timeout per gateway
        });
        if (response.ok) {
          console.log(`[Solana Balances] IPFS fetched from ${gateway}`);
          return response;
        }
      } catch {
        // Try next gateway
      }
    }
    return null;
  }

  // For non-IPFS URLs, just fetch directly
  return fetch(imageUrl, {
    headers: { "User-Agent": "OTC-Desk/1.0" },
    signal: AbortSignal.timeout(8000),
  }).catch(() => null);
}

/**
 * Cache an image URL to Vercel Blob storage
 * Returns the cached blob URL, or null if caching fails (don't return broken IPFS URLs)
 */
async function cacheImageToBlob(
  imageUrl: string | null,
): Promise<string | null> {
  if (!imageUrl) return null;

  // Skip if already a blob URL
  if (imageUrl.includes("blob.vercel-storage.com")) {
    return imageUrl;
  }

  try {
    const urlHash = crypto.createHash("md5").update(imageUrl).digest("hex");
    const extension = getExtensionFromUrl(imageUrl) || "png";
    const blobPath = `token-images/${urlHash}.${extension}`;

    // Check if already cached in blob storage
    const existing = await head(blobPath).catch(() => null);
    if (existing) {
      console.log(`[Solana Balances] Image already cached: ${existing.url}`);
      return existing.url;
    }

    // Download with gateway fallback for IPFS
    const response = await fetchWithIpfsGatewayFallback(imageUrl);

    if (!response || !response.ok) {
      console.warn(`[Solana Balances] Failed to fetch image: ${imageUrl}`);
      return null; // Return null instead of broken URL
    }

    const contentType = response.headers.get("content-type") || "image/png";
    const imageBuffer = await response.arrayBuffer();

    const blob = await put(blobPath, imageBuffer, {
      access: "public",
      contentType,
      addRandomSuffix: false,
      allowOverwrite: true,
    });

    console.log(`[Solana Balances] Cached image to blob: ${blob.url}`);
    return blob.url;
  } catch (error) {
    console.error("[Solana Balances] Image cache error:", error);
    return null; // Return null instead of broken URL
  }
}

function getExtensionFromUrl(url: string): string | null {
  try {
    const pathname = new URL(url).pathname;
    const match = pathname.match(/\.([a-zA-Z0-9]+)$/);
    if (match) {
      const ext = match[1].toLowerCase();
      if (["png", "jpg", "jpeg", "gif", "webp", "svg"].includes(ext)) {
        return ext;
      }
    }
    return null;
  } catch {
    return null;
  }
}

/**
 * Codex GraphQL response types
 */
interface CodexBalanceItem {
  balance: string;
  balanceUsd: string | null;
  shiftedBalance: number;
  tokenAddress: string;
  token: {
    name: string;
    symbol: string;
    address: string;
    decimals: number;
    networkId: number;
    info?: {
      imageSmallUrl: string | null;
    };
  } | null;
}

/**
 * Fetch balances from Codex API (faster, enriched data)
 */
async function fetchFromCodex(
  walletAddress: string,
  codexKey: string,
): Promise<CachedWalletResponse["tokens"] | null> {
  const query = `
    query GetBalances($input: BalancesInput!) {
      balances(input: $input) {
        items {
          balance
          balanceUsd
          shiftedBalance
          tokenAddress
          token {
            name
            symbol
            address
            decimals
            networkId
            info {
              imageSmallUrl
            }
          }
        }
      }
    }
  `;

  try {
    const response = await fetch(CODEX_GRAPHQL_URL, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: codexKey,
      },
      body: JSON.stringify({
        query,
        variables: {
          input: {
            walletAddress,
            networks: [SOLANA_NETWORK_ID],
            includeNative: true,
            removeScams: true,
            limit: 100,
          },
        },
      }),
      signal: AbortSignal.timeout(10000),
    });

    if (!response.ok) {
      console.log(`[Solana Balances] Codex returned ${response.status}`);
      return null;
    }

    const data = await response.json();
    if (data.errors) {
      console.log("[Solana Balances] Codex GraphQL errors:", data.errors);
      return null;
    }

    const items = data.data?.balances?.items as CodexBalanceItem[] | undefined;
    if (!items || items.length === 0) {
      console.log("[Solana Balances] Codex returned no items");
      return null;
    }

    console.log(`[Solana Balances] Codex returned ${items.length} tokens`);

    // Transform to our format
    const tokens = items
      .filter((item) => item.token) // Skip items without token metadata
      .map((item) => {
        const token = item.token;
        // For native SOL, use Wrapped SOL mint
        const mint =
          item.tokenAddress === "native"
            ? "So11111111111111111111111111111111111111112"
            : item.tokenAddress;

        return {
          mint,
          amount: parseInt(item.balance),
          decimals: token?.decimals ?? 9,
          symbol: token?.symbol ?? "SPL",
          name: token?.name ?? "Unknown",
          logoURI: token?.info?.imageSmallUrl ?? null,
          priceUsd:
            item.balanceUsd && item.shiftedBalance > 0
              ? parseFloat(item.balanceUsd) / item.shiftedBalance
              : 0,
          balanceUsd: item.balanceUsd ? parseFloat(item.balanceUsd) : 0,
        };
      })
      .filter(
        (t) =>
          t.balanceUsd >= 0.01 || t.amount > 100 * Math.pow(10, t.decimals),
      )
      .sort((a, b) => {
        if (a.balanceUsd > 0 && b.balanceUsd > 0)
          return b.balanceUsd - a.balanceUsd;
        if (a.balanceUsd > 0) return -1;
        if (b.balanceUsd > 0) return 1;
        return b.amount - a.amount;
      });

    return tokens;
  } catch (error) {
    console.error("[Solana Balances] Codex fetch error:", error);
    return null;
  }
}

/**
 * Fetch Solana token balances with cached metadata
 * Tries Codex first, falls back to Helius
 */
export async function GET(request: NextRequest) {
  const heliusKey = process.env.HELIUS_API_KEY;
  const codexKey = process.env.CODEX_API_KEY;
  const walletAddress = request.nextUrl.searchParams.get("address");
  const forceRefresh = request.nextUrl.searchParams.get("refresh") === "true";

  if (!walletAddress) {
    return NextResponse.json(
      { error: "Wallet address required" },
      { status: 400 },
    );
  }

  // Check wallet cache first (15 minute TTL) unless force refresh
  if (!forceRefresh) {
    const cachedTokens = await getCachedWalletResponse(walletAddress);
    if (cachedTokens) {
      return NextResponse.json({ tokens: cachedTokens });
    }
  } else {
    console.log("[Solana Balances] Force refresh requested");
  }

  // Try Codex first (faster, enriched data with USD values included)
  if (codexKey) {
    console.log("[Solana Balances] Trying Codex API...");
    const codexTokens = await fetchFromCodex(walletAddress, codexKey);
    if (codexTokens && codexTokens.length > 0) {
      console.log(
        `[Solana Balances] Codex returned ${codexTokens.length} tokens`,
      );
      await setCachedWalletResponse(walletAddress, codexTokens);
      return NextResponse.json({ tokens: codexTokens, source: "codex" });
    }
    console.log(
      "[Solana Balances] Codex returned no results, falling back to Helius",
    );
  }

  // Fall back to Helius
  if (!heliusKey) {
    return NextResponse.json({ tokens: [] });
  }

  try {
    // Step 1: Get token balances from Helius (fast, single call)
    const balancesResponse = await fetch(
      `https://mainnet.helius-rpc.com/?api-key=${heliusKey}`,
      {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          jsonrpc: "2.0",
          id: "balances",
          method: "getTokenAccountsByOwner",
          params: [
            walletAddress,
            { programId: "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA" },
            { encoding: "jsonParsed" },
          ],
        }),
        signal: AbortSignal.timeout(10000),
      },
    );

    if (!balancesResponse.ok) {
      console.error(
        "[Solana Balances] Helius balances error:",
        balancesResponse.status,
      );
      return NextResponse.json({ tokens: [] });
    }

    interface TokenAccount {
      pubkey: string;
      account: {
        data: {
          parsed: {
            info: {
              mint: string;
              tokenAmount: {
                amount: string;
                decimals: number;
                uiAmount: number;
              };
            };
          };
        };
      };
    }

    const balancesData = await balancesResponse.json();
    const accounts = (balancesData.result?.value || []) as TokenAccount[];

    console.log(
      `[Solana Balances] RPC returned ${accounts.length} token accounts`,
    );

    // Filter to tokens with balance > 0
    const tokensWithBalance = accounts
      .map((acc) => {
        const info = acc.account.data.parsed.info;
        const decimals = info.tokenAmount.decimals;
        const rawAmount = parseInt(info.tokenAmount.amount || "0");
        // Calculate humanBalance ourselves in case uiAmount is null
        const humanBalance =
          info.tokenAmount.uiAmount ?? rawAmount / Math.pow(10, decimals);
        return {
          mint: info.mint,
          amount: rawAmount,
          decimals,
          humanBalance,
        };
      })
      .filter((t) => t.amount > 0); // Any non-zero balance

    console.log(
      `[Solana Balances] Found ${tokensWithBalance.length} tokens with balance > 0`,
    );

    if (tokensWithBalance.length === 0) {
      return NextResponse.json({ tokens: [] });
    }

    // Step 2: Get metadata from cache first, then fetch missing from Helius
    interface HeliusAsset {
      id: string;
      content?: {
        metadata?: { name?: string; symbol?: string };
        links?: { image?: string };
      };
      token_info?: { symbol?: string; decimals?: number };
    }

    const allMints = tokensWithBalance.map((t) => t.mint);
    const cachedMetadata = await getSolanaMetadataCache();
    const metadata: Record<
      string,
      { symbol: string; name: string; logoURI: string | null }
    > = { ...cachedMetadata };

    // Find mints that need metadata
    const mintsNeedingMetadata = allMints.filter((mint) => !metadata[mint]);
    console.log(
      `[Solana Balances] ${Object.keys(cachedMetadata).length} cached, ${mintsNeedingMetadata.length} need metadata`,
    );

    // Batch fetch metadata for uncached tokens (100 at a time)
    if (mintsNeedingMetadata.length > 0) {
      for (let i = 0; i < mintsNeedingMetadata.length; i += 100) {
        const batch = mintsNeedingMetadata.slice(i, i + 100);
        try {
          const metadataResponse = await fetch(
            `https://mainnet.helius-rpc.com/?api-key=${heliusKey}`,
            {
              method: "POST",
              headers: { "Content-Type": "application/json" },
              body: JSON.stringify({
                jsonrpc: "2.0",
                id: "metadata",
                method: "getAssetBatch",
                params: { ids: batch },
              }),
              signal: AbortSignal.timeout(8000),
            },
          );

          if (metadataResponse.ok) {
            const data = await metadataResponse.json();
            const assets = (data.result || []) as HeliusAsset[];
            for (const asset of assets) {
              if (asset?.id) {
                metadata[asset.id] = {
                  symbol:
                    asset.content?.metadata?.symbol ||
                    asset.token_info?.symbol ||
                    "SPL",
                  name: asset.content?.metadata?.name || "Unknown",
                  logoURI: asset.content?.links?.image || null,
                };
              }
            }
          }
        } catch {
          // Continue without this batch's metadata
        }
      }

      // Update bulk metadata cache (merge with existing to handle concurrent requests)
      getSolanaMetadataCache()
        .then((existing) => {
          const merged = { ...existing, ...metadata };
          setSolanaMetadataCache(merged).catch((err) =>
            console.debug(
              "[Solana Balances] Metadata cache write failed:",
              err,
            ),
          );
        })
        .catch(() => {});
    }

    console.log(
      `[Solana Balances] Got metadata for ${Object.keys(metadata).length} tokens`,
    );

    // Step 3: Get prices from cache first, then fetch missing from Jupiter
    const mints = tokensWithBalance.map((t) => t.mint);
    const cachedPrices = await getSolanaPriceCache();
    const prices: Record<string, number> = { ...cachedPrices };

    // Find mints that need prices
    const mintsNeedingPrices = mints.filter(
      (mint) => prices[mint] === undefined,
    );
    console.log(
      `[Solana Balances] ${Object.keys(cachedPrices).length} prices cached, ${mintsNeedingPrices.length} need fetch`,
    );

    // Jupiter price API - fetch in batches of 100
    if (mintsNeedingPrices.length > 0) {
      for (let i = 0; i < mintsNeedingPrices.length; i += 100) {
        const batch = mintsNeedingPrices.slice(i, i + 100);
        try {
          const priceResponse = await fetch(
            `https://api.jup.ag/price/v2?ids=${batch.join(",")}`,
            { signal: AbortSignal.timeout(10000) },
          );

          if (priceResponse.ok) {
            const priceData = await priceResponse.json();
            if (priceData.data) {
              for (const [mint, data] of Object.entries(priceData.data)) {
                const price = (data as { price?: string })?.price;
                if (price) prices[mint] = parseFloat(price);
              }
            }
          } else {
            console.log(
              `[Solana Balances] Jupiter batch ${i / 100 + 1} returned ${priceResponse.status}`,
            );
          }
        } catch (err) {
          console.log(
            `[Solana Balances] Jupiter batch ${i / 100 + 1} failed:`,
            err,
          );
        }
      }

      // Update bulk price cache (merge with existing to handle concurrent requests)
      getSolanaPriceCache()
        .then((existing) => {
          const merged = { ...existing, ...prices };
          setSolanaPriceCache(merged).catch((err) =>
            console.debug("[Solana Balances] Price cache write failed:", err),
          );
        })
        .catch(() => {});
    }
    console.log(
      `[Solana Balances] Have prices for ${Object.keys(prices).length} tokens`,
    );

    // Step 4: Check blob cache for unreliable image URLs (parallel)
    const unreliableUrls = Object.values(metadata)
      .map((m) => m.logoURI)
      .filter(
        (url) =>
          url &&
          (url.includes("ipfs.io/ipfs/") ||
            url.includes("storage.auto.fun") ||
            url.includes(".mypinata.cloud")),
      ) as string[];

    const cachedBlobUrls: Record<string, string> = {};
    if (unreliableUrls.length > 0) {
      const blobChecks = await Promise.all(
        unreliableUrls.map(async (url) => {
          const urlHash = crypto.createHash("md5").update(url).digest("hex");
          const blobPath = `token-images/${urlHash}.png`;
          const existing = await head(blobPath).catch(() => null);
          return { url, blobUrl: existing?.url || null };
        }),
      );
      for (const { url, blobUrl } of blobChecks) {
        if (blobUrl) cachedBlobUrls[url] = blobUrl;
      }
    }
    console.log(
      `[Solana Balances] Found ${Object.keys(cachedBlobUrls).length} cached blob images`,
    );

    // Step 5: Combine everything
    const tokensWithData = tokensWithBalance.map((token) => {
      const meta = metadata[token.mint];
      const priceUsd = prices[token.mint] || 0;
      const rawLogoUrl = meta?.logoURI || null;

      // Get reliable URL: blob cache > reliable URL > null
      let logoURI: string | null = null;
      if (rawLogoUrl) {
        if (rawLogoUrl.includes("blob.vercel-storage.com")) {
          logoURI = rawLogoUrl;
        } else if (cachedBlobUrls[rawLogoUrl]) {
          logoURI = cachedBlobUrls[rawLogoUrl];
        } else if (
          !rawLogoUrl.includes("ipfs.io/ipfs/") &&
          !rawLogoUrl.includes("storage.auto.fun") &&
          !rawLogoUrl.includes(".mypinata.cloud")
        ) {
          logoURI = rawLogoUrl;
        }
      }

      return {
        mint: token.mint,
        amount: token.amount,
        decimals: token.decimals,
        humanBalance: token.humanBalance,
        priceUsd,
        balanceUsd: token.humanBalance * priceUsd,
        symbol: meta?.symbol || "SPL",
        name: meta?.name || "Unknown",
        logoURI,
        // Keep original URL for background caching
        _originalLogoUrl: rawLogoUrl,
      };
    });

    // Filter: only show tokens worth listing (>$0.01 or >100 tokens if no price)
    const MIN_USD_VALUE = 0.01;
    const MIN_TOKENS_NO_PRICE = 100;

    const filteredTokens = tokensWithData.filter((t) => {
      if (t.priceUsd > 0) return t.balanceUsd >= MIN_USD_VALUE;
      return t.humanBalance >= MIN_TOKENS_NO_PRICE;
    });

    // Sort: priced tokens by value, then unpriced by balance
    filteredTokens.sort((a, b) => {
      if (a.balanceUsd > 0 && b.balanceUsd > 0)
        return b.balanceUsd - a.balanceUsd;
      if (a.balanceUsd > 0) return -1;
      if (b.balanceUsd > 0) return 1;
      return b.humanBalance - a.humanBalance;
    });

    console.log(
      `[Solana Balances] ${tokensWithBalance.length} total -> ${filteredTokens.length} after filter`,
    );

    // Fire-and-forget: cache unreliable images in background for next request
    for (const token of filteredTokens.slice(0, 30)) {
      const originalUrl = (token as { _originalLogoUrl?: string })
        ._originalLogoUrl;
      if (originalUrl && !originalUrl.includes("blob.vercel-storage.com")) {
        cacheImageToBlob(originalUrl).catch(() => {});
      }
    }

    // Format response
    const enrichedTokens = filteredTokens.map((t) => ({
      mint: t.mint,
      amount: t.amount,
      decimals: t.decimals,
      priceUsd: t.priceUsd,
      balanceUsd: t.balanceUsd,
      symbol: t.symbol,
      name: t.name,
      logoURI: t.logoURI,
    }));

    // Cache for 15 minutes
    await setCachedWalletResponse(walletAddress, enrichedTokens);

    return NextResponse.json({ tokens: enrichedTokens });
  } catch (error) {
    console.error("[Solana Balances] Error:", error);
    return NextResponse.json({ tokens: [] });
  }
}
