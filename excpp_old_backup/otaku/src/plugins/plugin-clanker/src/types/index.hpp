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



// Constants

// Configuration schema

using ClankerConfig = z.infer<typeof ClankerConfigSchema>;

// Clanker SDK v4.0.0 Types
struct ClankerTokenMetadata {
    std::optional<std::string> description;
    std::optional<std::vector<std::string>> socialMediaUrls;
    std::optional<std::vector<std::string>> auditUrls;
};

struct ClankerTokenContext {
    std::optional<std::string> interface;
    std::optional<std::string> platform;
    std::optional<std::string> messageId;
    std::optional<std::string> id;
};

struct PoolPosition {
    double tickLower;
    double tickUpper;
    double positionBps;
};

struct PoolConfig {
    std::optional<std::string> pairedToken;
    std::optional<double> tickIfToken0IsClanker;
    std::optional<std::vector<PoolPosition>> positions;
};

struct StaticFeeConfig {
    "static" type;
    number; // in bps clankerFee;
    number; // in bps pairedFee;
};

struct DynamicFeeConfig {
    "dynamic" type;
};

using FeeConfig = std::variant<StaticFeeConfig, DynamicFeeConfig>;

struct RewardRecipient {
    std::string recipient;
    std::string admin;
    number; // basis points, sum must be 10000 (100%) bps;
    "Both" | "Paired" | "Clanker" token;
};

struct RewardsConfig {
    std::vector<RewardRecipient> recipients;
};

struct VaultConfig {
    number; // up to 90% percentage;
    number; // in seconds, min 7 days lockupDuration;
    number; // in seconds, can be 0 vestingDuration;
};

struct PoolKey {
    std::string currency0;
    std::string currency1;
    double fee;
    double tickSpacing;
    std::string hooks;
};

struct DevBuyConfig {
    number; // decimal amount of ETH to spend ethAmount;
    std::optional<PoolKey; // required for non-WETH paired tokens> poolKey;
    std::optional<number; // minimum amount out for WETH -> Paired swap> amountOutMin;
};

// Token deployment types for Clanker SDK v4.0.0
struct TokenDeployParams {
    std::string name;
    std::string symbol;
    std::optional<std::string> tokenAdmin;
    std::optional<bool> vanity;
    std::optional<std::string> image;
    std::optional<ClankerTokenMetadata> metadata;
    std::optional<ClankerTokenContext> context;
    std::optional<PoolConfig> pool;
    std::optional<FeeConfig> fees;
    std::optional<RewardsConfig> rewards;
    std::optional<VaultConfig> vault;
    std::optional<DevBuyConfig> devBuy;
};

struct DeployResult {
    std::string contractAddress;
    std::string transactionHash;
    string; // String representation of wei (bigint converted to string for JSON serialization) deploymentCost;
    std::optional<std::string> tokenId;
};

// Token information types
struct TokenInfo {
    std::string address;
    std::string name;
    std::string symbol;
    double decimals;
    bigint totalSupply;
    std::optional<double> price;
    std::optional<double> priceUsd;
    std::optional<double> volume24h;
    std::optional<double> holders;
    std::optional<double> liquidity;
    std::optional<double> marketCap;
    std::optional<double> createdAt;
    std::optional<std::string> creator;
};

// Legacy types for backward compatibility (deprecated)
struct LiquidityParams {
    std::string tokenA;
    std::string tokenB;
    BigNumberish amountA;
    BigNumberish amountB;
    std::optional<double> slippage;
    std::optional<double> deadline;
};

struct RemoveLiquidityParams {
    std::string lpToken;
    BigNumberish liquidity;
    BigNumberish minAmountA;
    BigNumberish minAmountB;
    std::optional<double> deadline;
};

struct LiquidityResult {
    bigint lpTokens;
    std::string transactionHash;
    [bigint, bigint] actualAmounts;
    std::optional<std::string> lpTokenAddress;
};

// Swap types (deprecated - use external DEX)
struct SwapParams {
    std::string fromToken;
    std::string toToken;
    BigNumberish amount;
    std::optional<double> slippage;
    std::optional<std::string> recipient;
    std::optional<double> deadline;
};

struct SwapRoute {
    std::string token;
    std::string pool;
    double fee;
};

struct SwapResult {
    bigint outputAmount;
    std::string transactionHash;
    std::vector<SwapRoute> route;
    double priceImpact;
    std::optional<bigint> gasUsed;
};

// Transaction types
struct Transaction {
    std::string to;
    std::optional<std::string> from;
    std::optional<BigNumberish> value;
    std::optional<std::string> data;
    std::optional<BigNumberish> gasLimit;
    std::optional<BigNumberish> gasPrice;
    std::optional<double> nonce;
};

struct SignedTransaction {
    std::string hash;
    std::string raw;
};

struct TransactionReceipt {
    std::string hash;
    double blockNumber;
    std::string blockHash;
    bigint gasUsed;
    bool status;
    std::vector<std::any> logs;
};

// Error types
struct ErrorResponse {
    std::string code;
    std::string message;
    std::optional<std::any> details;
    std::optional<std::vector<std::string>> suggestions;
};

enum ErrorCode {
  VALIDATION_ERROR = "VALIDATION_ERROR",
  NETWORK_ERROR = "NETWORK_ERROR",
  PROTOCOL_ERROR = "PROTOCOL_ERROR",
  SECURITY_ERROR = "SECURITY_ERROR",
  INSUFFICIENT_BALANCE = "INSUFFICIENT_BALANCE",
  SLIPPAGE_EXCEEDED = "SLIPPAGE_EXCEEDED",
  TRANSACTION_FAILED = "TRANSACTION_FAILED",
  UNAUTHORIZED = "UNAUTHORIZED",
}

// Action parameter schemas

// Transaction monitoring
struct TransactionStatus {
    std::string hash;
    "pending" | "confirmed" | "failed" status;
    double confirmations;
    std::optional<std::string> error;
};

// Balance types
struct TokenBalance {
    std::string token;
    std::string symbol;
    double decimals;
    bigint balance;
    std::string formattedBalance;
    std::optional<double> priceUsd;
    std::optional<double> valueUsd;
};

// Preset configurations (based on Clanker SDK documentation)


} // namespace elizaos
