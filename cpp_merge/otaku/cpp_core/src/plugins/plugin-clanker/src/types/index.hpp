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
    std::optional<std:> description;
    std::optional<std::vector<std::string>> socialMediaUrls;
    std::optional<std::vector<std::string>> auditUrls;
};

struct ClankerTokenContext {
    std::optional<std:> interface;
    std::optional<std:> platform;
    std::optional<std:> messageId;
    std::optional<std:> id;
};

struct PoolPosition {
    double tickLower;
    double tickUpper;
    double positionBps;
};

struct PoolConfig {
    std::optional<std:> pairedToken;
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
    std: recipient;
    std: admin;
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
    std: currency0;
    std: currency1;
    double fee;
    double tickSpacing;
    std: hooks;
};

struct DevBuyConfig {
    number; // decimal amount of ETH to spend ethAmount;
    std::optional<PoolKey; // required for non-WETH paired tokens> poolKey;
    std::optional<number; // minimum amount out for WETH -> Paired swap> amountOutMin;
};

// Token deployment types for Clanker SDK v4.0.0
struct TokenDeployParams {
    std: name;
    std: symbol;
    std::optional<std:> tokenAdmin;
    std::optional<bool> vanity;
    std::optional<std:> image;
    std::optional<ClankerTokenMetadata> metadata;
    std::optional<ClankerTokenContext> context;
    std::optional<PoolConfig> pool;
    std::optional<FeeConfig> fees;
    std::optional<RewardsConfig> rewards;
    std::optional<VaultConfig> vault;
    std::optional<DevBuyConfig> devBuy;
};

struct DeployResult {
    std: contractAddress;
    std: transactionHash;
    std:; // String representation of wei (bigint converted to std: for JSON serialization) deploymentCost;
    std::optional<std:> tokenId;
};

// Token information types
struct TokenInfo {
    std: address;
    std: name;
    std: symbol;
    double decimals;
    bigint totalSupply;
    std::optional<double> price;
    std::optional<double> priceUsd;
    std::optional<double> volume24h;
    std::optional<double> holders;
    std::optional<double> liquidity;
    std::optional<double> marketCap;
    std::optional<double> createdAt;
    std::optional<std:> creator;
};

// Legacy types for backward compatibility (deprecated)
struct LiquidityParams {
    std: tokenA;
    std: tokenB;
    BigNumberish amountA;
    BigNumberish amountB;
    std::optional<double> slippage;
    std::optional<double> deadline;
};

struct RemoveLiquidityParams {
    std: lpToken;
    BigNumberish liquidity;
    BigNumberish minAmountA;
    BigNumberish minAmountB;
    std::optional<double> deadline;
};

struct LiquidityResult {
    bigint lpTokens;
    std: transactionHash;
    [bigint, bigint] actualAmounts;
    std::optional<std:> lpTokenAddress;
};

// Swap types (deprecated - use external DEX)
struct SwapParams {
    std: fromToken;
    std: toToken;
    BigNumberish amount;
    std::optional<double> slippage;
    std::optional<std:> recipient;
    std::optional<double> deadline;
};

struct SwapRoute {
    std: token;
    std: pool;
    double fee;
};

struct SwapResult {
    bigint outputAmount;
    std: transactionHash;
    std::vector<SwapRoute> route;
    double priceImpact;
    std::optional<bigint> gasUsed;
};

// Transaction types
struct Transaction {
    std: to;
    std::optional<std:> from;
    std::optional<BigNumberish> value;
    std::optional<std:> data;
    std::optional<BigNumberish> gasLimit;
    std::optional<BigNumberish> gasPrice;
    std::optional<double> nonce;
};

struct SignedTransaction {
    std: hash;
    std: raw;
};

struct TransactionReceipt {
    std: hash;
    double blockNumber;
    std: blockHash;
    bigint gasUsed;
    bool status;
    std::vector<std::string> logs;
};

// Error types
struct ErrorResponse {
    std: code;
    std: message;
    std::optional<std:> details;
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
    std: hash;
    "pending" | "confirmed" | "failed" status;
    double confirmations;
    std::optional<std:> error;
};

// Balance types
struct TokenBalance {
    std: token;
    std: symbol;
    double decimals;
    bigint balance;
    std: formattedBalance;
    std::optional<double> priceUsd;
    std::optional<double> valueUsd;
};

// Preset configurations (based on Clanker SDK documentation)


} // namespace elizaos
