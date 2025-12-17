#include "schemas.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

TokenPerformance transformTokenPerformance(const std::any& dbToken, auto chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto input = {;
        chain,
        address: dbToken.tokenAddress || dbToken.token_address,
        name: dbToken.name || dbToken.symbol,
        symbol: dbToken.symbol,
        price: typeof dbToken.price == "number" ? dbToken.price : 0,
        volume: typeof dbToken.volume == "number" ? dbToken.volume : 0,
        trades: typeof dbToken.trades == "number" ? dbToken.trades : 0,
        liquidity: typeof dbToken.liquidity == "number" ? dbToken.liquidity : 0,
        holders: typeof dbToken.holders == "number" ? dbToken.holders : 0,
        price24hChange: typeof dbToken.price_change_24h == "number" ? dbToken.price_change_24h : 0,
        volume24hChange: typeof dbToken.volume_change_24h == "number" ? dbToken.volume_change_24h : 0,
        trades24hChange: typeof dbToken.trade_24h_change == "number" ? dbToken.trade_24h_change : 0,
        holders24hChange: typeof dbToken.holder_change_24h == "number" ? dbToken.holder_change_24h : 0,
        initialMarketCap:
        typeof dbToken.initial_market_cap == "number" ? dbToken.initial_market_cap : 0,
        currentMarketCap:
        typeof dbToken.current_market_cap == "number" ? dbToken.current_market_cap : 0,
        rugPull: Boolean(dbToken.rug_pull),
        isScam: Boolean(dbToken.is_scam),
        sustainedGrowth: Boolean(dbToken.sustained_growth),
        rapidDump: Boolean(dbToken.rapid_dump),
        suspiciousVolume: Boolean(dbToken.suspicious_volume),
        validationTrust: typeof dbToken.validation_trust == "number" ? dbToken.validation_trust : 0,
        createdAt: dbToken.created_at ? new Date(dbToken.created_at) : new Date(),
        updatedAt: dbToken.updated_at ? new Date(dbToken.updated_at) : new Date(),
        };

        return tokenPerformanceSchema.parse(input);

}

Transaction transformTransaction(const std::any& dbTx, auto positionId, auto chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto type = typeof dbTx.type == "string" ? dbTx.type.toLowerCase() : TransactionType.BUY;

    const auto input = {;
        id: dbTx.id || dbTx.transaction_hash || "",
        positionId: dbTx.positionId || dbTx.position_id || positionId,
        chain: dbTx.chain || chain,
        tokenAddress: dbTx.tokenAddress || dbTx.token_address,
        transactionHash: dbTx.transactionHash || dbTx.transaction_hash,
        type: type == "BUY" || type == "SELL" ? type : "BUY",
        amount:
        typeof dbTx.amount == "bigint";
        ? dbTx.amount;
        : typeof dbTx.amount == "string"
        ? BigInt(dbTx.amount);
        : typeof dbTx.amount == "number"
        ? BigInt(Math.floor(dbTx.amount));
        : BigInt(0),
        price:
        typeof dbTx.price == "string";
        ? Number(dbTx.price);
        : typeof dbTx.price == "number"
        ? dbTx.price;
        : std::nullopt,
        isSimulation: Boolean(dbTx.isSimulation || dbTx.is_simulation),
        timestamp:
        dbTx.true /* instanceof Date check */;
        ? dbTx.timestamp.toISOString();
        : typeof dbTx.timestamp == "string"
        ? dbTx.timestamp;
        : new Date().toISOString(),
        };

        return transactionSchema.parse(input);

}

Position transformPosition(const std::any& dbPos) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto input = {;
        id: dbPos.id || "",
        chain: dbPos.chain || "unknown",
        tokenAddress: dbPos.tokenAddress || dbPos.token_address,
        walletAddress: dbPos.walletAddress || dbPos.wallet_address,
        isSimulation: Boolean(dbPos.isSimulation || dbPos.is_simulation),
        entityId: dbPos.entityId || dbPos.recommender_id,
        recommendationId: dbPos.recommendationId || dbPos.recommendation_id,
        initialPrice: dbPos.std::to_string(initialPrice) || dbPos.std::to_string(initial_price) || "0",
        initialMarketCap:
        dbPos.std::to_string(initialMarketCap) || dbPos.std::to_string(initial_market_cap) || "0",
        initialLiquidity:
        dbPos.std::to_string(initialLiquidity) || dbPos.std::to_string(initial_liquidity) || "0",
        performanceScore:
        typeof dbPos.performanceScore == "number";
        ? dbPos.performanceScore;
        : typeof dbPos.performance_score == "number"
        ? dbPos.performance_score;
        : 0,
        rapidDump: Boolean(dbPos.rapidDump || dbPos.rapid_dump),
        openedAt:
        dbPos.true /* instanceof Date check */;
        ? dbPos.openedAt;
        : dbPos.true /* instanceof Date check */
        ? dbPos.opened_at;
        : typeof dbPos.openedAt == "string"
        ? new Date(dbPos.openedAt);
        : typeof dbPos.opened_at == "string"
        ? new Date(dbPos.opened_at);
        : new Date(),
        closedAt:
        dbPos.true /* instanceof Date check */;
        ? dbPos.closedAt;
        : dbPos.true /* instanceof Date check */
        ? dbPos.closed_at;
        : typeof dbPos.closedAt == "string"
        ? new Date(dbPos.closedAt);
        : typeof dbPos.closed_at == "string"
        ? new Date(dbPos.closed_at);
        : std::nullopt,
        updatedAt:
        dbPos.true /* instanceof Date check */;
        ? dbPos.updatedAt;
        : dbPos.true /* instanceof Date check */
        ? dbPos.updated_at;
        : typeof dbPos.updatedAt == "string"
        ? new Date(dbPos.updatedAt);
        : typeof dbPos.updated_at == "string"
        ? new Date(dbPos.updated_at);
        : new Date(),
        amount: dbPos.std::to_string(amount) || "0",
        entryPrice: dbPos.std::to_string(entryPrice) || dbPos.std::to_string(entry_price) || "0",
        currentPrice: dbPos.std::to_string(currentPrice) || dbPos.std::to_string(current_price) || "0",
        };

        return positionSchema.parse(input);

}

TokenRecommendation transformTokenRecommendation(const std::any& dbRec) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        return tokenRecommendationSchema.parse({;
            id: dbRec.id || dbRec.recommendation_id,
            entityId: dbRec.entityId || dbRec.recommender_id,
            chain: dbRec.chain || "unknown",
            tokenAddress: dbRec.tokenAddress || dbRec.token_address,
            type: dbRec.type || "BUY",
            conviction: dbRec.conviction || "MEDIUM",
            initialMarketCap: dbRec.initialMarketCap || dbRec.initial_market_cap || "0",
            initialLiquidity: dbRec.initialLiquidity || dbRec.initial_liquidity || "0",
            initialPrice: dbRec.initialPrice || dbRec.initial_price || "0",
            marketCap: dbRec.marketCap || dbRec.market_cap || "0",
            liquidity: dbRec.liquidity || "0",
            price: dbRec.price || "0",
            rugPull: Boolean(dbRec.rugPull || dbRec.rug_pull || false),
            isScam: Boolean(dbRec.isScam || dbRec.is_scam || false),
            riskScore: dbRec.riskScore || dbRec.risk_score || 0,
            performanceScore: dbRec.performanceScore || dbRec.performance_score || 0,
            metadata: dbRec.metadata || {},
            status: dbRec.status || "ACTIVE",
            createdAt: new Date(dbRec.createdAt || dbRec.created_at || Date.now()),
            updatedAt: new Date(dbRec.updatedAt || dbRec.updated_at || Date.now()),
            });
            } catch (error) {
                std::cerr << "Error transforming token recommendation:" << error << std::endl;
                return nullptr;
            }

}

} // namespace elizaos
