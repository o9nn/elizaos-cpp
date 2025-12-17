#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Configuration for the Community Trader Plugin
 *
 * This file centralizes all configuration options for the trading system.
 */

/**
 * Buy amount configuration
 */
struct BuyAmountConfig {
    double baseAmount;
    double minAmount;
    double maxAmount;
    double trustScoreMultiplier;
    double convictionMultiplier;
};

/**
 * Trading configuration
 */
struct TradingConfig {
    double slippageBps;
    bool forceSimulation;
    std::string defaultChain;
    double maxPositionsPerToken;
    double maxPositionsPerRecommender;
    double minLiquidityUsd;
    double maxMarketCapUsd;
    BuyAmountConfig buyAmountConfig;
};

/**
 * Database configuration
 */
struct DatabaseConfig {
    std::string schemaVersion;
    bool enableCaching;
    double cacheTimeout;
};

/**
 * Memory configuration
 */
struct MemoryConfig {
    std::string embeddingModel;
    double embeddingDimension;
    double similarityThreshold;
    double cacheTimeout;
};

/**
 * Default trading configuration
 */

/**
 * Default database configuration
 */

/**
 * Default memory configuration
 */

/**
 * Conviction levels for recommendations
 * IMPORTANT: Must match the enum in types.ts
 */
enum Conviction {
}

/**
 * Recommendation types
 * IMPORTANT: Must match the enum in types.ts
 */
enum RecommendationType {
}

/**
 * Transaction types
 * IMPORTANT: Must match the enum in types.ts
 */
enum TransactionType {
}

/**
 * Buy signal message interface
 */
struct BuySignalMessage {
    std::string tokenAddress;
    std::optional<std::string> chain;
    std::optional<std::string> walletAddress;
    std::optional<bool> isSimulation;
    UUID entityId;
    std::optional<UUID> recommendationId;
    std::optional<Conviction> conviction;
    std::optional<std::string> price;
    std::optional<std::string> marketCap;
    std::optional<std::string> liquidity;
    std::optional<RecommendationType> type;
};

/**
 * Sell signal message interface
 */
struct SellSignalMessage {
    UUID positionId;
    std::string tokenAddress;
    UUID sellRecommenderId;
    std::optional<std::string> walletAddress;
    std::optional<bool> isSimulation;
};

/**
 * Utility functions for configuration
 */

/**
 * Get conviction multiplier
 */
double getConvictionMultiplier(Conviction conviction);

/**
 * Get liquidity multiplier
 */
double getLiquidityMultiplier(double liquidity);

/**
 * Get market cap multiplier
 */
double getMarketCapMultiplier(double marketCap);

/**
 * Get volume multiplier
 */
double getVolumeMultiplier(double volume);

} // namespace elizaos
