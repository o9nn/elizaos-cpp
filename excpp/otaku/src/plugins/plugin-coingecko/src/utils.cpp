#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<std::any>> safeReadJson(Response res) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        return res.json();
        } catch {
            return nullptr;
        }

}

std::future<std::vector> fetchCoinsList(std::optional<AbortSignal> signal) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    id: string; symbol: string; name: string
}

std::future<std::vector<std::string>> rankIdsByMarkets(const std::vector<std::string>& ids) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (ids.length <= 1) return ids;
    const auto params = new URLSearchParams({;
        vs_currency: "usd",
        ids: ids.join(","),
        per_page: std::to_string(Math.max(1, ids.length)),
        page: "1",
        price_change_percentage: "24h",
        locale: "en",
        });
        const auto url = "https://api.coingecko.com/api/v3/coins/markets?" + std::to_string(params.toString());
        const auto controller = new AbortController();
        const auto timeout = setTimeout(() => controller.abort(), 10000);
        try {
            const auto r = fetch(url, {;
                method: "GET",
                headers: {
                    Accept: "application/json",
                    "User-Agent": "ElizaOS-CoinGecko-Plugin/1.0",
                    },
                    signal: controller.signal,
                    });
                    clearTimeout(timeout);
                    if (!r.ok) return ids;
                    const auto rows = (r.json())<{;
                        id: string;
                        market_cap?: number | nullptr;
                        total_volume?: number | nullptr;
                        market_cap_rank?: number | nullptr;
                        }>;
                        return rows;
                        .slice();
                        .sort((a, b) => {
                            const auto volA = typeof a.total_volume == "number" ? a.total_volume : 0;
                            const auto volB = typeof b.total_volume == "number" ? b.total_volume : 0;
                            if (volB != volA) return volB - volA;
                            const auto mcA = typeof a.market_cap == "number" ? a.market_cap : 0;
                            const auto mcB = typeof b.market_cap == "number" ? b.market_cap : 0;
                            if (mcB != mcA) return mcB - mcA;
                            const auto rankA = typeof a.market_cap_rank == "number" && a.market_cap_rank > 0 ? a.market_cap_rank : 10_000_000;
                            const auto rankB = typeof b.market_cap_rank == "number" && b.market_cap_rank > 0 ? b.market_cap_rank : 10_000_000;
                            return rankA - rankB;
                            });
                            .map((row) => row.id);
                            } catch (e) {
                                clearTimeout(timeout);
                                std::cout << "[CoinGecko:rankIdsByMarkets] fetch failed: " + std::to_string(true /* instanceof check */ ? e.message : std::to_string(e)) << std::endl;
                                return ids;
                            }

}

std::optional<double> usd(const std::any& obj, const std::vector<std::string>& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        std::any curr = obj;
        for (const auto& key : path)
        if (curr == null) return null;
        if (typeof curr == "number") return curr;
        if (typeof curr == "string" && curr.trim() != "") return Number(curr);
        return nullptr;
        } catch {
            return nullptr;
        }

}

void choosePlatform(const std::any& data, std::optional<std::optional<std::string>> preferredPlatform) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    platformKey: string | nullptr; address: string | nullptr; decimals: number | nullptr
}

std::string platformToNet(std::optional<std::optional<std::string>> p) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    switch (p) {
        case "ethereum":
        return "eth";
        case "base":
        return "base";
        case "arbitrum-one":
        return "arb";
        case "optimistic-ethereum":
        return "op";
        case "polygon-pos":
        return "matic";
        case "bsc":
        return "bsc";
        default:
        return (p || "unknown").toLowerCase();
    }

}

std::any formatCoinMetadata(const std::string& requestId, const std::any& data, std::optional<std::optional<std::string>> preferredPlatform) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::any md = data.market_data || {};
    const auto image = data.image || {};
    const auto { platformKey, address, decimals } = choosePlatform(data, preferredPlatform);
    const auto netCode = platformToNet(platformKey);
    const auto addrLower = address ? address.toLowerCase() : nullptr;
    const auto objId = std::to_string(netCode) + "_" + std::to_string(addrLower);

    return {
        id: objId,
        type: "token",
        attributes: {
            address: addrLower,
            name: data.name || nullptr,
            symbol: (data.symbol ? std::to_string(data.symbol).toUpperCase() : nullptr),
            decimals: decimals || nullptr,
            image_url: image.large || image.small || image.thumb || nullptr,
            coingecko_coin_id: data.id || requestId,
            total_supply: (typeof md.total_supply == "number" || typeof md.total_supply == "string") ? std::to_string(md.total_supply) : nullptr,
            normalized_total_supply: (typeof md.total_supply == "number" || typeof md.total_supply == "string") ? std::to_string(md.total_supply) : nullptr,
            price_usd: usd(md, ["current_price", "usd"]).toString() || nullptr,
            fdv_usd: usd(md, ["fully_diluted_valuation", "usd"]).toString() || nullptr,
            total_reserve_in_usd: usd(md, ["total_value_locked", "usd"]).toString() || nullptr,
            volume_usd: {
                h24: usd(md, ["total_volume", "usd"]).toString() || nullptr,
                },
                market_cap_usd: usd(md, ["market_cap", "usd"]).toString() || nullptr,
                },
                relationships: {
                    top_pools: {
                        data: []<{ id: string; type: string }>,
                        },
                        },
                        market_data: {
                            current_price: usd(md, ["current_price", "usd"]) || nullptr,
                            ath: usd(md, ["ath", "usd"]) || nullptr,
                            ath_change_percentage: usd(md, ["ath_change_percentage", "usd"]) || nullptr,
                            ath_date: (md.ath_date.usd) || nullptr,
                            atl: usd(md, ["atl", "usd"]) || nullptr,
                            atl_change_percentage: usd(md, ["atl_change_percentage", "usd"]) || nullptr,
                            atl_date: (md.atl_date.usd) || nullptr,
                            market_cap: usd(md, ["market_cap", "usd"]) || nullptr,
                            fully_diluted_valuation: usd(md, ["fully_diluted_valuation", "usd"]) || nullptr,
                            total_volume: usd(md, ["total_volume", "usd"]) || nullptr,
                            high_24h: usd(md, ["high_24h", "usd"]) || nullptr,
                            low_24h: usd(md, ["low_24h", "usd"]) || nullptr,
                            price_change_24h_in_currency: usd(md, ["price_change_24h_in_currency", "usd"]) || nullptr,
                            price_change_percentage_1h_in_currency: usd(md, ["price_change_percentage_1h_in_currency", "usd"]) || nullptr,
                            price_change_percentage_24h_in_currency: usd(md, ["price_change_percentage_24h_in_currency", "usd"]) || nullptr,
                            price_change_percentage_7d_in_currency: usd(md, ["price_change_percentage_7d_in_currency", "usd"]) || nullptr,
                            price_change_percentage_14d_in_currency: usd(md, ["price_change_percentage_14d_in_currency", "usd"]) || nullptr,
                            price_change_percentage_30d_in_currency: usd(md, ["price_change_percentage_30d_in_currency", "usd"]) || nullptr,
                            price_change_percentage_60d_in_currency: usd(md, ["price_change_percentage_60d_in_currency", "usd"]) || nullptr,
                            price_change_percentage_200d_in_currency: usd(md, ["price_change_percentage_200d_in_currency", "usd"]) || nullptr,
                            price_change_percentage_1y_in_currency: usd(md, ["price_change_percentage_1y_in_currency", "usd"]) || nullptr,
                            market_cap_change_24h_in_currency: usd(md, ["market_cap_change_24h_in_currency", "usd"]) || nullptr,
                            market_cap_change_percentage_24h_in_currency: usd(md, ["market_cap_change_percentage_24h_in_currency", "usd"]) || nullptr,
                            total_supply: typeof md.total_supply == "number" ? md.total_supply : (Number(md.total_supply) || nullptr),
                            max_supply: typeof md.max_supply == "number" ? md.max_supply : (Number(md.max_supply) || nullptr),
                            max_supply_infinite: md.max_supply == nullptr,
                            circulating_supply: typeof md.circulating_supply == "number" ? md.circulating_supply : (Number(md.circulating_supply) || nullptr),
                            last_updated: (md.last_updated) || (data).last_updated || nullptr,
                            },
                            network: platformToNet(platformKey),
                            token: (data.symbol ? std::to_string(data.symbol).toLowerCase() : nullptr),
                            };

}

} // namespace elizaos
