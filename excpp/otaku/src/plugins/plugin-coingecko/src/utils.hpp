#include "elizaos/core.hpp"
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

std::future<any | null> safeReadJson(Response res); catch {
    return null;
  }
}

async >> {
  const url = "https://api.coingecko.com/api/v3/coins/list";
  const r = await fetch(url, {
    method: "GET",
    headers: {
      Accept: "application/json",
      "User-Agent": "ElizaOS-CoinGecko-Plugin/1.0",
    },
    signal,
  });
  if (!r.ok) {
    const body = await safeReadJson(r);
    throw new Error(`Failed to load coins list ${r.status}: ${r.statusText}${body ? ` - ${JSON.stringify(body)}` : ""}`);
  }
  return (await r.json()) as Array<{ id: string; symbol: string; name: string }>;
}

std::future<std::vector<std::string>> rankIdsByMarkets(const std::vector<std::string>& ids););
  const url = `https://api.coingecko.com/api/v3/coins/markets?${params.toString()}`;
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 10000);
  try {
    const r = await fetch(url, {
      method: "GET",
      headers: {
        Accept: "application/json",
        "User-Agent": "ElizaOS-CoinGecko-Plugin/1.0",
      },
      signal: controller.signal,
    });
    clearTimeout(timeout);
    if (!r.ok) return ids;
    const rows = (await r.json()) as Array<{
      id: string;
      market_cap?: number | null;
      total_volume?: number | null;
      market_cap_rank?: number | null;
    }>;
    return rows
      .slice()
      .sort((a, b) => {
        const volA = typeof a.total_volume === "number" ? a.total_volume : 0;
        const volB = typeof b.total_volume === "number" ? b.total_volume : 0;
        if (volB !== volA) return volB - volA;
        const mcA = typeof a.market_cap === "number" ? a.market_cap : 0;
        const mcB = typeof b.market_cap === "number" ? b.market_cap : 0;
        if (mcB !== mcA) return mcB - mcA;
        const rankA = typeof a.market_cap_rank === "number" && a.market_cap_rank > 0 ? a.market_cap_rank : 10_000_000;
        const rankB = typeof b.market_cap_rank === "number" && b.market_cap_rank > 0 ? b.market_cap_rank : 10_000_000;
        return rankA - rankB;
      })
      .map((row) => row.id);
  } catch (e) {
    clearTimeout(timeout);
    logger.warn(`[CoinGecko:rankIdsByMarkets] fetch failed: ${e instanceof Error ? e.message : String(e)}`);
    return ids;
  }
}

 catch {
    return null;
  }
}

 {
  const detail = data?.detail_platforms || {};
  const platforms = data?.platforms || {};
  const preferred = ["ethereum", "base", "arbitrum-one", "optimistic-ethereum", "polygon-pos", "bsc"];
  // If caller hints a specific platform (e.g., resolved via contract endpoint), honor it first
  if (preferredPlatform && (detail[preferredPlatform] || platforms[preferredPlatform])) {
    const d = detail[preferredPlatform];
    const address: string | null = (d?.contract_address as string) || (platforms?.[preferredPlatform] as string) || null;
    const decimals: number | null = typeof d?.decimal_place === "number" ? d.decimal_place : null;
    return { platformKey: preferredPlatform, address, decimals };
  }
  const keys = Object.keys(detail).length > 0 ? Object.keys(detail) : Object.keys(platforms);
  const pick = preferred.find((k) => keys.includes(k)) || keys[0] || null;
  if (!pick) return { platformKey: null, address: null, decimals: null };
  const d = detail[pick];
  const address: string | null = (d?.contract_address as string) || (platforms?.[pick] as string) || null;
  const decimals: number | null = typeof d?.decimal_place === "number" ? d.decimal_place : null;
  return { platformKey: pick, address, decimals };
}


}

;
  const image = data?.image || {};
  const { platformKey, address, decimals } = choosePlatform(data, preferredPlatform);
  const netCode = platformToNet(platformKey);
  const addrLower = address ? address.toLowerCase() : null;
  const objId = addrLower ? `${netCode}_${addrLower}` : (data?.id as string);

  return {
    id: objId,
    type: "token",
    attributes: {
      address: addrLower,
      name: data?.name ?? null,
      symbol: (data?.symbol ? String(data.symbol).toUpperCase() : null),
      decimals: decimals ?? null,
      image_url: image.large || image.small || image.thumb || null,
      coingecko_coin_id: data?.id ?? requestId,
      total_supply: (typeof md.total_supply === "number" || typeof md.total_supply === "string") ? String(md.total_supply) : null,
      normalized_total_supply: (typeof md.total_supply === "number" || typeof md.total_supply === "string") ? String(md.total_supply) : null,
      price_usd: usd(md, ["current_price", "usd"])?.toString() ?? null,
      fdv_usd: usd(md, ["fully_diluted_valuation", "usd"])?.toString() ?? null,
      total_reserve_in_usd: usd(md, ["total_value_locked", "usd"])?.toString() ?? null,
      volume_usd: {
        h24: usd(md, ["total_volume", "usd"])?.toString() ?? null,
      },
      market_cap_usd: usd(md, ["market_cap", "usd"])?.toString() ?? null,
    },
    relationships: {
      top_pools: {
        data: [] as Array<{ id: string; type: string }>,
      },
    },
    market_data: {
      current_price: usd(md, ["current_price", "usd"]) ?? null,
      ath: usd(md, ["ath", "usd"]) ?? null,
      ath_change_percentage: usd(md, ["ath_change_percentage", "usd"]) ?? null,
      ath_date: (md.ath_date?.usd as string) || null,
      atl: usd(md, ["atl", "usd"]) ?? null,
      atl_change_percentage: usd(md, ["atl_change_percentage", "usd"]) ?? null,
      atl_date: (md.atl_date?.usd as string) || null,
      market_cap: usd(md, ["market_cap", "usd"]) ?? null,
      fully_diluted_valuation: usd(md, ["fully_diluted_valuation", "usd"]) ?? null,
      total_volume: usd(md, ["total_volume", "usd"]) ?? null,
      high_24h: usd(md, ["high_24h", "usd"]) ?? null,
      low_24h: usd(md, ["low_24h", "usd"]) ?? null,
      price_change_24h_in_currency: usd(md, ["price_change_24h_in_currency", "usd"]) ?? null,
      price_change_percentage_1h_in_currency: usd(md, ["price_change_percentage_1h_in_currency", "usd"]) ?? null,
      price_change_percentage_24h_in_currency: usd(md, ["price_change_percentage_24h_in_currency", "usd"]) ?? null,
      price_change_percentage_7d_in_currency: usd(md, ["price_change_percentage_7d_in_currency", "usd"]) ?? null,
      price_change_percentage_14d_in_currency: usd(md, ["price_change_percentage_14d_in_currency", "usd"]) ?? null,
      price_change_percentage_30d_in_currency: usd(md, ["price_change_percentage_30d_in_currency", "usd"]) ?? null,
      price_change_percentage_60d_in_currency: usd(md, ["price_change_percentage_60d_in_currency", "usd"]) ?? null,
      price_change_percentage_200d_in_currency: usd(md, ["price_change_percentage_200d_in_currency", "usd"]) ?? null,
      price_change_percentage_1y_in_currency: usd(md, ["price_change_percentage_1y_in_currency", "usd"]) ?? null,
      market_cap_change_24h_in_currency: usd(md, ["market_cap_change_24h_in_currency", "usd"]) ?? null,
      market_cap_change_percentage_24h_in_currency: usd(md, ["market_cap_change_percentage_24h_in_currency", "usd"]) ?? null,
      total_supply: typeof md.total_supply === "number" ? md.total_supply : (Number(md.total_supply) || null),
      max_supply: typeof md.max_supply === "number" ? md.max_supply : (Number(md.max_supply) || null),
      max_supply_infinite: md.max_supply == null,
      circulating_supply: typeof md.circulating_supply === "number" ? md.circulating_supply : (Number(md.circulating_supply) || null),
      last_updated: (md.last_updated as string) || (data as any)?.last_updated || null,
    },
    network: platformToNet(platformKey),
    token: (data?.symbol ? String(data.symbol).toLowerCase() : null),
  };
}



} // namespace elizaos
