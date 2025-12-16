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

// Constants
const NATIVE_TOKEN_ADDRESSES =
  "0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE";

// Configuration schema
const ClankerConfigSchema = z.object({
  BASE_RPC_URL: z.string().min(1, "Base RPC URL is required"),
  DEFAULT_SLIPPAGE: z.number().default(0.05), // 5%
  MAX_GAS_PRICE: z.string().default("100000000000"), // 100 gwei
  RETRY_ATTEMPTS: z.number().default(3),
  NETWORK: z.enum(["base", "base-sepolia"]).default("base"),
});

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
const TokenDeploySchema = z.object({
  name: z.string().min(1).max(50),
  symbol: z.string().min(1).max(10).toUpperCase(),
  tokenAdmin: z
    .string()
    .regex(/^0x[a-fA-F0-9]{40}$/)
    .optional(),
  vanity: z.boolean().optional(),
  image: z.string().optional(),
  metadata: z
    .object({
      description: z.string().optional(),
      socialMediaUrls: z.array(z.string().url()).optional(),
      auditUrls: z.array(z.string().url()).optional(),
    })
    .optional(),
  context: z
    .object({
      interface: z.string().optional(),
      platform: z.string().optional(),
      messageId: z.string().optional(),
      id: z.string().optional(),
    })
    .optional(),
  pool: z
    .object({
      pairedToken: z
        .string()
        .regex(/^0x[a-fA-F0-9]{40}$/)
        .optional(),
      tickIfToken0IsClanker: z.number().optional(),
      positions: z
        .array(
          z.object({
            tickLower: z.number(),
            tickUpper: z.number(),
            positionBps: z.number(),
          }),
        )
        .optional(),
    })
    .optional(),
  fees: z
    .union([
      z.object({
        type: z.literal("static"),
        clankerFee: z.number(),
        pairedFee: z.number(),
      }),
      z.object({
        type: z.literal("dynamic"),
      }),
    ])
    .optional(),
  rewards: z
    .object({
      recipients: z.array(
        z.object({
          recipient: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
          admin: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
          bps: z.number().min(0).max(10000),
          token: z.enum(["Both", "Paired", "Clanker"]),
        }),
      ),
    })
    .optional(),
  vault: z
    .object({
      percentage: z.number().min(0).max(90),
      lockupDuration: z.number().min(604800), // min 7 days in seconds
      vestingDuration: z.number().min(0),
    })
    .optional(),
  devBuy: z
    .object({
      ethAmount: z.number().min(0),
      poolKey: z
        .object({
          currency0: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
          currency1: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
          fee: z.number(),
          tickSpacing: z.number(),
          hooks: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
        })
        .optional(),
      amountOutMin: z.number().optional(),
    })
    .optional(),
});

const SwapSchema = z.object({
  fromToken: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
  toToken: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
  amount: z.string().regex(/^\d+$/),
  slippage: z.number().min(0).max(0.5).optional(),
  recipient: z
    .string()
    .regex(/^0x[a-fA-F0-9]{40}$/)
    .optional(),
});

const LiquiditySchema = z.object({
  tokenA: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
  tokenB: z.string().regex(/^0x[a-fA-F0-9]{40}$/),
  amountA: z.string().regex(/^\d+$/),
  amountB: z.string().regex(/^\d+$/),
  slippage: z.number().min(0).max(0.5).optional(),
});

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
const POOL_POSITIONS = {
  Standard: [
    { tickLower: -60000, tickUpper: -20000, positionBps: 8000 },
    { tickLower: -20000, tickUpper: 100000, positionBps: 2000 },
  ],
  Project: [
    { tickLower: -60000, tickUpper: -20000, positionBps: 6000 },
    { tickLower: -20000, tickUpper: 60000, positionBps: 3000 },
    { tickLower: 60000, tickUpper: 100000, positionBps: 1000 },
  ],
};

const FEE_CONFIGS = {
  StaticBasic: {
    type: "static" as const,
    clankerFee: 100, // 1%
    pairedFee: 100, // 1%
  },
  DynamicBasic: {
    type: "dynamic" as const,
  },
};

} // namespace elizaos
