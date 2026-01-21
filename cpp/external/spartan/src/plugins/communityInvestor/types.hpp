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


// Re-UUID type for use in other files
/**
 * Represents a universally unique identifier (UUID).
 */
using UUID = CoreUUID;

/**
 * Represents a type where certain properties from the original type T are optional.
 * @template T - The original type
 * @template K - The keys of the properties that should be optional
 * @typedef {Omit<T, K> & Partial<Pick<T, K>>} Optional
 */
/**
 * Creates a new type by transforming each key in the provided type `type` into a property with the same key and value.
 * @template type The type to make pretty.
 * @typedef {Object} Pretty
 * @property {keyof type} key The key from the original type
 * @property {type[key]} value The value associated with the key from the original type
 * @augments unknown
 */

/**
 * Type that extracts variables enclosed in double curly braces from a given string.
 *
 * @template T The input string type
 * @typedef {T} ExtractVariables
 * @param {T} T The input string to extract variables from
 * @returns {Var} The variables extracted from the input string
 */

/**
 * Represents a type that defines template variables for a given string type.
 *
 * @template T - The string type for which template variables are defined.
 * @typedef TemplateVariables
 * @type {Pretty<{ [K in ExtractVariables<T>]: string; }>}
 */

/**
 * Represents a value that can be stored in a SQLite database, which can be a string, number, or null.
 */
using SQLiteValue = std::variant<std::string, double, nullptr>;

/**
 * Type utility for converting TypeScript types to SQLite column types.
 *
 * @template T - The TypeScript type to convert.
 * @param {T} - The value to convert.
 * @returns {ToSQLiteType<T>} - The SQLite column type equivalent of the input type.
 */

/**
 * Converts a generic record type to a SQLite record type, where each property value is converted to a SQLite type.
 *
 * @template T - The generic record type to be converted to a SQLite record type.
 * @typedef ToSQLiteRecord
 * @type {object}
 */

/**
 * Represents a type which is used to define a single row in the database table for RecommenderMetrics.
 */
using RecommenderMetricsRow = ToSQLiteRecord<RecommenderMetrics>;
/**
 * Defines an alias for converting a TokenPerformance object into a SQLite record format.
 */
using TokenPerformanceRow = ToSQLiteRecord<TokenPerformance>;
/**
 * Represents a single row of data in a SQLite database table, corresponding to the Position model.
 */
using PositionRow = ToSQLiteRecord<Position>;
/**
 * A type alias representing a row in the Transaction table,
 * serialized as a SQLite record.
 */
using TransactionRow = ToSQLiteRecord<Transaction>;

/**
 * Interface representing the metrics of a recommender.
 * @typedef {{
 *    entityId: UUID,
 *    platform: string,
 *    totalRecommendations: number,
 *    successfulRecs: number,
 *    failedTrades: number,
 *    totalProfit: number,
 *    avgTokenPerformance: number,
 *    consistencyScore: number,
 *    trustScore: number,
 *    lastUpdated: Date,
 *    createdAt: Date
 * }} RecommenderMetrics
 */
struct RecommenderMetrics {
    UUID entityId;
    std::string platform;
    double totalRecommendations;
    double successfulRecs;
    double failedTrades;
    double totalProfit;
    double avgTokenPerformance;
    double consistencyScore;
    double trustScore;
    Date lastUpdated;
    Date createdAt;
};

/**
 * Interface representing the history of recommender metrics for a specific entity.
 * @typedef {Object} RecommenderMetricsHistory
 * @property {UUID} entityId - The ID of the entity for which the metrics are recorded.
 * @property {RecommenderMetrics} metrics - The metrics related to the entity.
 * @property {Date} timestamp - The timestamp when the metrics were recorded.
 */
struct RecommenderMetricsHistory {
    UUID entityId;
    RecommenderMetrics metrics;
    Date timestamp;
};

/**
 * Interface representing performance data for a token.
 * @typedef {Object} TokenPerformance
 * @property {string} [chain] - The blockchain network the token belongs to.
 * @property {string} [address] - The address of the token.
 * @property {string} [name] - The name of the token.
 * @property {string} [symbol] - The symbol of the token.
 * @property {number} [decimals] - The number of decimal places for the token.
 * @property {Object.<string, any>} [metadata] - Additional metadata for the token.
 * @property {number} [price] - The current price of the token.
 * @property {number} [price24hChange] - The percentage change in price over the last 24 hours.
 * @property {number} [volume] - The trading volume of the token.
 * @property {number} [volume24hChange] - The percentage change in trading volume over the last 24 hours.
 * @property {number} [trades] - The number of trades for the token.
 * @property {number} [trades24hChange] - The percentage change in number of trades over the last 24 hours.
 * @property {number} [liquidity] - The liquidity of the token.
 * @property {number} [holders] - The number of holders of the token.
 * @property {number} [holders24hChange] - The percentage change in number of holders over the last 24 hours.
 * @property {number} [initialMarketCap] - The initial market capitalization of the token.
 * @property {number} [currentMarketCap] - The current market capitalization of the token.
 * @property {boolean} [rugPull] - Indicates if the token is associated with a rug pull.
 * @property {boolean} [isScam] - Indicates if the token is considered a scam.
 * @property {boolean} [sustainedGrowth] - Indicates if the token has shown sustained growth.
 * @property {boolean} [rapidDump] - Indicates if the token has experienced a rapid dump in price.
 * @property {boolean} [suspiciousVolume] - Indicates if the token has suspicious trading volume.
 * @property {number} [validationTrust] - The level of trust in the token's validation.
 * @property {Date} [createdAt] - The date and time when the token performance data was created.
 * @property {Date} [updatedAt] - The date and time when the token performance data was last updated.
 */
struct TokenPerformance {
    std::optional<std::string> chain;
    std::optional<std::string> address;
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<double> decimals;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<double> price;
    std::optional<double> price24hChange;
    std::optional<double> volume;
    std::optional<double> volume24hChange;
    std::optional<double> trades;
    std::optional<double> trades24hChange;
    std::optional<double> liquidity;
    std::optional<double> holders;
    std::optional<double> holders24hChange;
    std::optional<double> initialMarketCap;
    std::optional<double> currentMarketCap;
    std::optional<bool> rugPull;
    std::optional<bool> isScam;
    std::optional<bool> sustainedGrowth;
    std::optional<bool> rapidDump;
    std::optional<bool> suspiciousVolume;
    std::optional<double> validationTrust;
    std::optional<Date> createdAt;
    std::optional<Date> updatedAt;
};

/**
 * Conviction levels for recommendations
 * IMPORTANT: Must match the enum in config.ts
 */
/**
 * Enumeration representing levels of conviction.
 * @readonly
 * @enum {string}
 * @property {string} NONE - No conviction.
 * @property {string} LOW - Low level of conviction.
 * @property {string} MEDIUM - Medium level of conviction.
 * @property {string} HIGH - High level of conviction.
 * @property {string} VERY_HIGH - Very high level of conviction.
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
 * IMPORTANT: Must match the enum in config.ts
 */
enum RecommendationType {
  BUY = 'BUY',
  DONT_BUY = 'DONT_BUY',
  SELL = 'SELL',
  DONT_SELL = 'DONT_SELL',
  NONE = 'NONE',
  HOLD = 'HOLD',
}

using TokenRecommendation = {

struct Position {
    UUID id;
    UUID entityId;
    std::string tokenAddress;
    std::string chain;
    std::string walletAddress;
    std::string balance;
    'OPEN' | 'CLOSED' status;
    Date createdAt;
    std::optional<Date> closedAt;
    bool isSimulation;
    std::string amount;
    std::string initialPrice;
    std::optional<std::string> currentPrice;
    UUID recommendationId;
};

using PositionWithBalance = Position & {

/**
 * Unified transaction type enums to ensure consistency
 * IMPORTANT: Must match the enum in config.ts
 */
enum TransactionType {
  BUY = 'BUY',
  SELL = 'SELL',
  TRANSFER_IN = 'transfer_in',
  TRANSFER_OUT = 'transfer_out',
}

/**
 * Complete transaction interface with all possible fields
 */
struct Transaction {
    UUID id;
    UUID positionId;
    std::string tokenAddress;
    TransactionType type;
    std::string amount;
    std::optional<double> valueUsd;
    std::optional<double> marketCap;
    std::optional<double> liquidity;
    std::string price;
    bool isSimulation;
    Date timestamp;
    std::optional<std::string> chain;
    std::optional<std::string> transactionHash;
};

using SellDetails = {

using BuyData = {

using SellData = {

using RecommenderAnalytics = {

using TokenRecommendationSummary = {

using TransactionData = {

using QuoteInParams = {

using TokenMetadata = {

using TokenMarketData = {

// Message recommendation extracted from text
struct MessageRecommendation {
    std::string tokenMentioned;
    bool isTicker;
    'positive' | 'negative' | 'neutral' sentiment;
    'NONE' | 'LOW' | 'MEDIUM' | 'HIGH' conviction;
    std::string quote;
};

// TODO: Consolidate this into "Entity" with metadata
using Account = {

using TokenTradeData = {

using HolderData = {

using TokenSecurityData = {

using ProcessedTokenData = {

using DexScreenerPair = {

using DexScreenerData = {

using Prices = {

using CalculatedBuyAmounts = {

using WalletPortfolioItem = {

using WalletPortfolio = {

using TokenOverview = {

struct BuySignalMessage {
    std::optional<std::string> positionId;
    std::string tokenAddress;
    std::string chain;
    std::string walletAddress;
    bool isSimulation;
    std::string entityId;
    std::string recommendationId;
    std::string price;
    std::string marketCap;
    std::string liquidity;
    std::string amount;
    RecommendationType type;
    Conviction conviction;
};

struct Trade {
    std::string id;
    std::string positionId;
    TransactionType.BUY | TransactionType.SELL type;
    bigint amount;
    bigint price;
    Date timestamp;
    std::string txHash;
};

struct TradePerformance {
    std::string token_address;
    std::string recommender_id;
    double buy_price;
    double sell_price;
    std::string buy_timeStamp;
    std::string sell_timeStamp;
    double buy_amount;
    double sell_amount;
    double buy_sol;
    double received_sol;
    double buy_value_usd;
    double sell_value_usd;
    double profit_usd;
    double profit_percent;
    double buy_market_cap;
    double sell_market_cap;
    double market_cap_change;
    double buy_liquidity;
    double sell_liquidity;
    double liquidity_change;
    std::string last_updated;
    bool rapidDump;
};

/**
 * Represents the metrics of a trade including total bought quantity, total bought value, total sold quantity,
 * total sold value, total transfer in quantity, total transfer out quantity, average entry price, average exit price,
 * realized profit and loss, realized profit and loss percentage, volume in USD, first trade time, and last trade time.
 * @typedef {Object} TradeMetrics
 * @property {number} totalBought - The total quantity bought
 * @property {number} totalBoughtValue - The total value of items bought
 * @property {number} totalSold - The total quantity sold
 * @property {number} totalSoldValue - The total value of items sold
 * @property {number} totalTransferIn - The total quantity transferred in
 * @property {number} totalTransferOut - The total quantity transferred out
 * @property {number} averageEntryPrice - The average price at which items were bought
 * @property {number} averageExitPrice - The average price at which items were sold
 * @property {number} realizedPnL - The realized profit and loss
 * @property {number} realizedPnLPercent - The realized profit and loss percentage
 * @property {number} volumeUsd - The volume in USD
 * @property {Date} firstTradeTime - The timestamp of the first trade
 * @property {Date} lastTradeTime - The timestamp of the last trade
 */
using TradeMetrics = {

/**
 * Type for position performance statistics.
 * Includes information about the position such as token, current value, initial value, profit/loss, profit/loss percentage,
 * price change, price change percentage, normalized balance, trade metrics, unrealized profit/loss, unrealized profit/loss percentage,
 * total profit/loss, and total profit/loss percentage.
 */
using PositionPerformance = Pretty<

// ServiceType Enum to identify the service within the runtime
enum ServiceType {
  COMMUNITY_INVESTOR = 'community-investor',
}

// Supported cryptocurrency chains
enum SupportedChain {
  SOLANA = 'SOLANA',
  ETHEREUM = 'ETHEREUM',
  BASE = 'BASE',
  UNKNOWN = 'UNKNOWN', // For cases where chain can't be determined
}

// Metrics calculated after observing a recommendation's market performance
struct RecommendationMetric {
    std::optional<number; // e.g., based on ATH after recommendation or price after X days for BUYs> potentialProfitPercent;
    std::optional<number; // For SELL/criticism, based on price drop avoided> avoidedLossPercent;
    std::optional<boolean; // Flagged based on heuristics> isScamOrRug;
    number; // When this metric was last calculated evaluationTimestamp;
    std::optional<string; // e.g., "Hit ATH 3 days later", "Rug pulled", "Low liquidity spike"> notes;
};

// Represents a single recommendation or criticism made by a user
struct Recommendation {
    UUID; // Unique ID for this recommendation instance id;
    UUID; // Entity ID of the recommender userId;
    UUID; // Original message ID that sparked this recommendation messageId;
    number; // When the recommendation was made (from original message) timestamp;
    std::optional<string; // e.g., "SOL", "BTC" (if identified as a ticker)> tokenTicker;
    string; // e.g., "So11111111111111111111111111111111111111112" tokenAddress;
    SupportedChain; // The blockchain the token is on chain;
    'BUY' | 'SELL'; // 'SELL' for criticisms recommendationType;
    'NONE' | 'LOW' | 'MEDIUM' | 'HIGH'; // Sender's conviction level conviction;
    string; // The exact text snippet that is the recommendation/criticism rawMessageQuote;
    std::optional<number; // Price of the token when the recommendation was made> priceAtRecommendation;
    std::optional<RecommendationMetric; // Performance metrics, calculated later by a task> metrics;
    std::optional<boolean; // Has the PROCESS_TRADE_DECISION task run for this?> processedForTradeDecision;
};

// Data structure for the component stored on an Entity
struct UserTrustProfile {
    string; // Schema version, e.g., "1.0.0" version;
    UUID; // Entity ID this profile belongs to userId;
    number; // Weighted average score from -100 to 100 trustScore;
    number; // When trustScore was last calculated lastTrustScoreCalculationTimestamp;
    std::optional<number; // For the 12-hour cooldown for *acting* on this user's recs> lastTradeDecisionMadeTimestamp;
    Recommendation[]; // Array of recommendations made by this user recommendations;
};

// Type alias for the data field within the ElizaOS Component
using TrustMarketplaceComponentData = UserTrustProfile;

// Constant for the component type name used in runtime.getComponent/createComponent

// Payload for the PROCESS_TRADE_DECISION task
struct TradeDecisionInput {
    UUID recommendationId;
    UUID userId;
};

// Structure for data fetched from external token APIs (e.g., Birdeye, DexScreener)
struct TokenAPIData {
    std::optional<std::string> name;
    std::optional<std::string> symbol;
    std::optional<double> currentPrice;
    std::optional<number; // All-Time High> ath;
    std::optional<number; // All-Time Low> atl;
    std::optional<number; // Price snapshot when the recommendation was made (if fetched at that time)> priceAtRecommendation;
    std::optional<std::vector<{ timestamp: number; price: number }>> priceHistory;

// Data structure for frontend leaderboard entries
struct LeaderboardEntry {
    UUID userId;
    std::optional<string; // Display name for the user> username;
    double trustScore;
    std::optional<number; // Calculated dynamically> rank;
    Recommendation[]; // Full recommendation history for drill-down recommendations;
};

// Interface defining the methods our CommunityInvestorService will provide
// This helps ensure the class implements all necessary functions.
struct ICommunityInvestorService {
    std::string ticker;
    SupportedChain chain;
    std::vector<Memory> contextMessages;
    Recommendation recommendation;
    TokenAPIData tokenData;
};

// Adding MessageReceivedHandlerParams
struct MessageReceivedHandlerParams {
    IAgentRuntime runtime;
    Memory message;
    ( callback;
    string | Record<string, any> response;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
    std::optional<() => void> onComplete;
};


} // namespace elizaos
