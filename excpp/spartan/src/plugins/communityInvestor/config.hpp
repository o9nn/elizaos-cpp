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
 * Configuration for the Community Trader Plugin
 *
 * This file centralizes all configuration options for the trading system.
 */

import type { UUID } from './types';

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
    number; // in seconds cacheTimeout;
};


/**
 * Memory configuration
 */
struct MemoryConfig {
    std::string embeddingModel;
    double embeddingDimension;
    double similarityThreshold;
    number; // in seconds cacheTimeout;
};


/**
 * Default trading configuration
 */
const DEFAULT_TRADING_CONFIG: TradingConfig = {
  slippageBps: 100, // 1%
  forceSimulation: false,
  defaultChain: 'solana',
  maxPositionsPerToken: 3,
  maxPositionsPerRecommender: 5,
  minLiquidityUsd: 10000, // $10k
  maxMarketCapUsd: 100000000, // $100M
  buyAmountConfig: {
    baseAmount: 0.1, // 0.1 SOL
    minAmount: 0.01, // 0.01 SOL
    maxAmount: 1.0, // 1 SOL
    trustScoreMultiplier: 0.5,
    convictionMultiplier: 0.3,
  },
};

/**
 * Default database configuration
 */
const DEFAULT_DATABASE_CONFIG: DatabaseConfig = {
  schemaVersion: '1.0',
  enableCaching: true,
  cacheTimeout: 3600, // 1 hour
};

/**
 * Default memory configuration
 */
const DEFAULT_MEMORY_CONFIG: MemoryConfig = {
  embeddingModel: 'text-embedding-ada-002',
  embeddingDimension: 1536,
  similarityThreshold: 0.7,
  cacheTimeout: 3600, // 1 hour
};

/**
 * Conviction levels for recommendations
 * IMPORTANT: Must match the enum in types.ts
 */
enum Conviction {
  NONE = 'NONE',
  LOW = 'LOW',
  MEDIUM = 'MEDIUM',
  HIGH = 'HIGH',
  VERY_HIGH = 'VERY_HIGH',
}

/**
 * Recommendation types
 * IMPORTANT: Must match the enum in types.ts
 */
enum RecommendationType {
  BUY = 'BUY',
  DONT_BUY = 'DONT_BUY',
  SELL = 'SELL',
  DONT_SELL = 'DONT_SELL',
  NONE = 'NONE',
  HOLD = 'HOLD',
}

/**
 * Transaction types
 * IMPORTANT: Must match the enum in types.ts
 */
enum TransactionType {
  BUY = 'BUY',
  SELL = 'SELL',
  TRANSFER_IN = 'transfer_in',
  TRANSFER_OUT = 'transfer_out',
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

}

/**
 * Get liquidity multiplier
 */


/**
 * Get market cap multiplier
 */


/**
 * Get volume multiplier
 */


} // namespace elizaos
