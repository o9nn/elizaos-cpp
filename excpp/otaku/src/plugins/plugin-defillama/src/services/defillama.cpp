#include "defillama.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<YieldPool> shapeYieldPool(RawYieldPool raw) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (
    typeof raw.pool != "string" ||;
    typeof raw.project != "string" ||;
    typeof raw.chain != "string" ||;
    typeof raw.symbol != "string";
    ) {
        return nullptr;
    }

    if (typeof raw.tvlUsd != "number" || !Number.isFinite(raw.tvlUsd)) {
        return nullptr;
    }

    const auto rewardTokens = Array.isArray(raw.rewardTokens);
    ? raw.rewardTokens.filter((token) => typeof token == "string");
    : nullptr;

    const auto underlyingTokens = Array.isArray(raw.underlyingTokens);
    ? raw.underlyingTokens.filter((token) => typeof token == "string");
    : nullptr;

    return {
        pool: raw.pool,
        project: raw.project,
        chain: raw.chain,
        symbol: raw.symbol,
        tvlUsd: raw.tvlUsd,
        apy: normalizeNullableNumber(raw.apy),
        apyBase: normalizeNullableNumber(raw.apyBase),
        apyReward: normalizeNullableNumber(raw.apyReward),
        rewardTokens,
        stablecoin: Boolean(raw.stablecoin),
        underlyingTokens,
        apyPct1D: normalizeNullableNumber(raw.apyPct1D),
        apyPct7D: normalizeNullableNumber(raw.apyPct7D),
        apyPct30D: normalizeNullableNumber(raw.apyPct30D),
        apyMean30d: normalizeNullableNumber(raw.apyMean30d),
        };

}

std::optional<double> normalizeNullableNumber(const std::variant<double, std::string>& value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value == "number" && Number.isFinite(value)) {
        return value;
    }
    if (typeof value == "string" && value.trim()) {
        const auto parsed = Number(value);
        if (!Number.isNaN(parsed) && Number.isFinite(parsed)) {
            return parsed;
        }
    }
    return nullptr;

}

ProtocolSummary shapeProtocol(DefiLlamaProtocol p) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> chains = Array.isArray(p.chains) ? Array.from(new Set(p.chains)) : [];
    const auto chainTvls = shapeChainTvlsRecord((p as { chainTvls?: ChainTvlsRawRecord }).chainTvls);

    const auto toNumberOrNull = (value: number | string | nullptr | std::nullopt): number | nullptr =>;
    typeof value == "number" && Number.isFinite(value) ? value : nullptr;

    const auto slugValue = typeof p.slug == "string" ? p.slug : nullptr;
    const auto urlValue = typeof p.url == "string" ? p.url : nullptr;
    const auto logoValue = typeof p.logo == "string" ? p.logo : nullptr;
    const auto categoryValue = typeof p.category == "string" ? p.category : nullptr;
    const auto addressValue = typeof p.address == "string" ? p.address : nullptr;
    const auto geckoValue = typeof p.gecko_id == "string" ? p.gecko_id : nullptr;
    const auto cmcValue = typeof p.cmcId == "string" ? p.cmcId : nullptr;
    const auto twitterValue = typeof p.twitter == "string" ? p.twitter : nullptr;
    const auto symbolValue = typeof p.symbol == "string" ? p.symbol : nullptr;

    return {
        id: p.id,
        slug: slugValue,
        name: p.name,
        symbol: symbolValue,
        url: urlValue,
        logo: logoValue,
        category: categoryValue,
        chains,
        address: addressValue,
        geckoId: geckoValue,
        cmcId: cmcValue,
        twitter: twitterValue,
        tvl: toNumberOrNull((p as { tvl?: number }).tvl),
        tvlChange1h: toNumberOrNull((p as { change_1h?: number }).change_1h),
        tvlChange1d: toNumberOrNull((p as { change_1d?: number }).change_1d),
        tvlChange7d: toNumberOrNull((p as { change_7d?: number }).change_7d),
        chainTvls,
        };

}

std::unordered_map<std::string, double> shapeChainTvlsRecord(ChainTvlsRawRecord value) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!value) {
        return {}
    }

    const std::unordered_map<std::string, double> shaped = {};
    for (const int [chainName, rawValue] of Object.entries(value)) {
        if (typeof rawValue == "number" && Number.isFinite(rawValue)) {
            shaped[chainName] = rawValue;
            } else if (typeof rawValue == "string") {
                const auto parsed = Number(rawValue);
                if (!Number.isNaN(parsed)) {
                    shaped[chainName] = parsed;
                }
                } else if (rawValue && typeof rawValue == "object" && typeof rawValue.tvl == "number" && Number.isFinite(rawValue.tvl)) {
                    shaped[chainName] = rawValue.tvl;
                }
            }
            return shaped;

}

ProtocolTvlHistory shapeProtocolHistory(RawProtocolHistory raw, const std::string& fallbackSlug) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<ProtocolTvlPoint> totalSeries = Array.isArray(raw.tvl);
    ? raw.tvl.filter(isRawProtocolHistoryPoint).map((point) => ({
        date: point.date,
        totalLiquidityUsd: point.totalLiquidityUSD,
        }));
        : [];

        const std::unordered_map<std::string, std::vector<ProtocolTvlPoint>> chainSeries = {};
        if (raw.chainTvls) {
            for (const int [chainName, chainData] of Object.entries(raw.chainTvls)) {
                const auto series = Array.isArray(chainData.tvl);
                ? chainData.tvl.filter(isRawProtocolHistoryPoint).map((point) => ({
                    date: point.date,
                    totalLiquidityUsd: point.totalLiquidityUSD,
                    }));
                    : [];

                    if (series.length > 0) {
                        chainSeries[chainName] = series;
                    }
                }
            }

            const auto latestPoint = totalSeries.length > 0 ? totalSeries[totalSeries.length - 1] : std::nullopt;

            return {
                slug: raw.slug || fallbackSlug,
                name: raw.name || fallbackSlug,
                symbol: raw.symbol || nullptr,
                currentTvl: latestPoint ? latestPoint.totalLiquidityUsd : nullptr,
                lastUpdated: latestPoint ? latestPoint.date : nullptr,
                totalSeries,
                chainSeries,
                };

}

point is isRawProtocolHistoryPoint(RawProtocolHistoryPoint point) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    date: number; totalLiquidityUSD: number
}

point is isRawChainTvlPoint(RawChainTvlPoint point) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    date: number; tvl: number
}

} // namespace elizaos
