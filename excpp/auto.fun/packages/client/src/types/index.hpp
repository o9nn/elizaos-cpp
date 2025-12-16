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

using HomepageSortBy = std::variant<"all", "marketCap", "newest", "oldest">;

struct IPagination {
    double page;
    double totalPages;
    double total;
    bool hasMore;
};


struct ISwap {
    double amountIn;
    double amountOut;
    0 | 1 direction;
    std::string id;
    double price;
    string | Date timestamp;
    std::string tokenMint;
    std::string txId;
    std::string type;
    std::string user;
};


struct ITokenHolder {
    std::string id;
    std::string mint;
    std::string address;
    double amount;
    double percentage;
    string | Date lastUpdated;
};


using ChartTable = {
  table: {
    open: number;
    high: number;
    low: number;
    close: number;
    time: number;
    volume: number;
  }[];
};

const TokenSchema = z
  .object({
    name: z.string().nullish(),
    url: z.string().nullish(),
    ticker: z.string().nullish(),
    createdAt: z.string().datetime(),
    mint: z.string(),
    image: z.string().nullish(),
    marketCapUSD: z.number().nullish(),
    currentPrice: z.number().nullish().default(0),
    curveProgress: z.number().nullish(),
    status: z
      .enum([
        "pending",
        "active",
        "withdrawn",
        "migrating",
        "migrated",
        "locked",
        "harvested",
        "migration_failed",
        "partner_import",
        "undefined",
      ])
      .nullish(),
    liquidity: z.number().nullish(),
    curveLimit: z.number().nullish().default(0),
    reserveLamport: z.number().nullish(),
    virtualReserves: z.number().nullish(),
    solPriceUSD: z.number().nullish(),
    holderCount: z.number().nullish().default(0),
    description: z.string().nullish(),
    discord: z.string().nullish(),
    twitter: z.string().nullish(),
    telegram: z.string().nullish(),
    farcaster: z.string().nullish(),
    creator: z.string().nullish(),
    volume24h: z.number().nullish(),
    website: z.string().nullish(),
    tokenPriceUSD: z.number().nullish(),
    tokenSupplyUiAmount: z.preprocess((val) => {
      if (typeof val === "string") {
        const num = Number(val);
        return isNaN(num) ? 0 : num;
      }
      return val;
    }, z.number()),
    tokenDecimals: z.number().nullish(),
    nftMinted: z.string().nullish(),
    lockId: z.string().nullish(),
    lockedAmount: z.string().nullish(),
    lockedAt: z.string().datetime().nullish(),
    harvestedAt: z.string().datetime().nullish(),
    completedAt: z.string().datetime().nullish(),
    withdrawnAt: z.string().datetime().nullish(),
    migratedAt: z.string().datetime().nullish(),
    marketId: z.string().nullish(),
    baseVault: z.string().nullish(),
    quoteVault: z.string().nullish(),
    withdrawnAmount: z.number().nullish(),
    reserveAmount: z.number().nullish(),
    priceChange24h: z.number().nullish(),
    price24hAgo: z.number().nullish(),
    inferenceCount: z.number().nullish(),
    lastVolumeReset: z.string().datetime().nullish(),
    lastPriceUpdate: z.string().datetime().nullish(),
    txId: z.string().nullish(),
    lastUpdated: z.string().datetime().nullish(),
    imported: z.preprocess((val) => {
      if (typeof val === "string") {
        const num = Number(val);
        return isNaN(num) ? 0 : num;
      }
      return val;
    }, z.number().nullish()),
    verified: z.number().nullish(),
    featured: z.number().nullish(),
    hidden: z.preprocess((val) => {
      if (typeof val === "string") {
        const num = Number(val);
        return isNaN(num) ? 0 : num;
      }
      return val;
    }, z.number().nullish()),
    isToken2022: z.preprocess((val) => {
      if (typeof val === "string") return parseInt(val, 10) || 0;
      return val ?? 0;
    }, z.number().optional().default(0)),
    hideFromFeatured: z.preprocess((val) => {
      if (typeof val === "string") return parseInt(val, 10) || 0;
      return val ?? 0;
    }, z.number().optional().default(0)),
    creatorProfile: z
      .object({
        address: z.string(),
        displayName: z.string().nullable(),
        profilePictureUrl: z.string().nullable(),
      })
      .optional()
      .nullable(),
  })
  .transform((data) => ({
    ...data,
    mint: data.mint,
    createdAt: data.createdAt,
    creator: data.creator,
    currentPrice: data.currentPrice != null ? Number(data.currentPrice) : 0,
    curveLimit: data.curveLimit != null ? Number(data.curveLimit) : 0,
    curveProgress: data.curveProgress != null ? Number(data.curveProgress) : 0,
    description: data.description || "",
    image: data.image || "",
    inferenceCount:
      data.inferenceCount != null ? Number(data.inferenceCount) : 0,
    lastUpdated: data.lastUpdated,
    liquidity: data.liquidity != null ? Number(data.liquidity) : 0,
    marketCapUSD: data.marketCapUSD != null ? Number(data.marketCapUSD) : 0,
    name: data.name,
    price24hAgo: data.price24hAgo != null ? Number(data.price24hAgo) : 0,
    priceChange24h:
      data.priceChange24h != null ? Number(data.priceChange24h) : 0,
    reserveAmount: data.reserveAmount != null ? Number(data.reserveAmount) : 0,
    reserveLamport:
      data.reserveLamport != null ? Number(data.reserveLamport) : 0,
    solPriceUSD: data.solPriceUSD != null ? Number(data.solPriceUSD) : 0,
    status: data.status || "active",
    telegram: data.telegram || "",
    farcaster: data.farcaster || "",
    ticker: data.ticker,
    tokenPriceUSD: data.tokenPriceUSD != null ? Number(data.tokenPriceUSD) : 0,
    twitter: data.twitter || "",
    txId: data.txId || "",
    url: data.url || "",
    discord: data?.discord || "",
    virtualReserves:
      data.virtualReserves != null ? Number(data.virtualReserves) : 0,
    volume24h: data.volume24h != null ? Number(data.volume24h) : 0,
    website: data.website || "",
    holderCount: data.holderCount != null ? Number(data.holderCount) : 0,
    lastPriceUpdate: data.lastPriceUpdate || data.lastUpdated,
    lastVolumeReset: data.lastVolumeReset || data.lastUpdated,
    imported: data.imported != null ? Number(data.imported) : 0,
    verified: data?.verified ? data?.verified : 0,
    featured: data?.featured ? data?.featured : 0,
    hidden: data?.hidden ? !!data.hidden : false,
    isToken2022: data.isToken2022 || 0,
    hideFromFeatured: data.hideFromFeatured || 0,
    creatorProfile: data.creatorProfile,
  }));

using IToken = z.infer<typeof TokenSchema>;

using ConfigAccount = {
  authority: PublicKey;
  pendingAuthority: PublicKey;
  teamWallet: PublicKey;
  initBondingCurve: number;
  platformBuyFee: BN;
  platformSellFee: BN;
  curveLimit: BN;
};

// Type definitions for global objects

declare global {
  struct Window {
    std::optional<{> solana;
    std::optional<bool> isPhantom;
    std::optional<(> signMessage;
    Uint8Array message;
    std::string encoding;
    () => Promise<{ publicKey: string }> connect;
    std::optional<() => Promise<void>> disconnect;
    std::optional<PublicKey> publicKey;
    std::optional<(event: string, callback: () => void) => void> on;
    std::optional<(event: string, callback: () => void) => void> off;
};

}

{};

} // namespace elizaos
