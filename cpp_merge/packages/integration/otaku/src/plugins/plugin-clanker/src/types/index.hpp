#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CLANKER_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PLUGINS_PLUGIN-CLANKER_SRC_TYPES_INDEX_H
#include "core.h"
#include "ethers.h"
#include "zod.h"

typedef z::infer<ClankerConfigSchema> ClankerConfig;

typedef any FeeConfig;

class ClankerTokenMetadata;
class ClankerTokenContext;
class PoolPosition;
class PoolConfig;
class StaticFeeConfig;
class DynamicFeeConfig;
class RewardRecipient;
class RewardsConfig;
class VaultConfig;
class PoolKey;
class DevBuyConfig;
class TokenDeployParams;
class DeployResult;
class TokenInfo;
class LiquidityParams;
class RemoveLiquidityParams;
class LiquidityResult;
class SwapParams;
class SwapRoute;
class SwapResult;
class Transaction;
class SignedTransaction;
class TransactionReceipt;
class ErrorResponse;
enum struct ErrorCode;
class TransactionStatus;
class TokenBalance;

extern string NATIVE_TOKEN_ADDRESSES;
extern any ClankerConfigSchema;
class ClankerTokenMetadata : public object, public std::enable_shared_from_this<ClankerTokenMetadata> {
public:
    using std::enable_shared_from_this<ClankerTokenMetadata>::shared_from_this;
    string description;

    array<string> socialMediaUrls;

    array<string> auditUrls;
};

class ClankerTokenContext : public object, public std::enable_shared_from_this<ClankerTokenContext> {
public:
    using std::enable_shared_from_this<ClankerTokenContext>::shared_from_this;
    string interface;

    string platform;

    string messageId;

    string id;
};

class PoolPosition : public object, public std::enable_shared_from_this<PoolPosition> {
public:
    using std::enable_shared_from_this<PoolPosition>::shared_from_this;
    double tickLower;

    double tickUpper;

    double positionBps;
};

class PoolConfig : public object, public std::enable_shared_from_this<PoolConfig> {
public:
    using std::enable_shared_from_this<PoolConfig>::shared_from_this;
    string pairedToken;

    double tickIfToken0IsClanker;

    array<std::shared_ptr<PoolPosition>> positions;
};

class StaticFeeConfig : public object, public std::enable_shared_from_this<StaticFeeConfig> {
public:
    using std::enable_shared_from_this<StaticFeeConfig>::shared_from_this;
    string type;

    double clankerFee;

    double pairedFee;
};

class DynamicFeeConfig : public object, public std::enable_shared_from_this<DynamicFeeConfig> {
public:
    using std::enable_shared_from_this<DynamicFeeConfig>::shared_from_this;
    string type;
};

class RewardRecipient : public object, public std::enable_shared_from_this<RewardRecipient> {
public:
    using std::enable_shared_from_this<RewardRecipient>::shared_from_this;
    string recipient;

    string admin;

    double bps;

    any token;
};

class RewardsConfig : public object, public std::enable_shared_from_this<RewardsConfig> {
public:
    using std::enable_shared_from_this<RewardsConfig>::shared_from_this;
    array<std::shared_ptr<RewardRecipient>> recipients;
};

class VaultConfig : public object, public std::enable_shared_from_this<VaultConfig> {
public:
    using std::enable_shared_from_this<VaultConfig>::shared_from_this;
    double percentage;

    double lockupDuration;

    double vestingDuration;
};

class PoolKey : public object, public std::enable_shared_from_this<PoolKey> {
public:
    using std::enable_shared_from_this<PoolKey>::shared_from_this;
    string currency0;

    string currency1;

    double fee;

    double tickSpacing;

    string hooks;
};

class DevBuyConfig : public object, public std::enable_shared_from_this<DevBuyConfig> {
public:
    using std::enable_shared_from_this<DevBuyConfig>::shared_from_this;
    double ethAmount;

    std::shared_ptr<PoolKey> poolKey;

    double amountOutMin;
};

class TokenDeployParams : public object, public std::enable_shared_from_this<TokenDeployParams> {
public:
    using std::enable_shared_from_this<TokenDeployParams>::shared_from_this;
    string name;

    string symbol;

    string tokenAdmin;

    boolean vanity;

    string image;

    std::shared_ptr<ClankerTokenMetadata> metadata;

    std::shared_ptr<ClankerTokenContext> context;

    std::shared_ptr<PoolConfig> pool;

    FeeConfig fees;

    std::shared_ptr<RewardsConfig> rewards;

    std::shared_ptr<VaultConfig> vault;

    std::shared_ptr<DevBuyConfig> devBuy;
};

class DeployResult : public object, public std::enable_shared_from_this<DeployResult> {
public:
    using std::enable_shared_from_this<DeployResult>::shared_from_this;
    string contractAddress;

    string transactionHash;

    string deploymentCost;

    string tokenId;
};

class TokenInfo : public object, public std::enable_shared_from_this<TokenInfo> {
public:
    using std::enable_shared_from_this<TokenInfo>::shared_from_this;
    string address;

    string name;

    string symbol;

    double decimals;

    any totalSupply;

    double price;

    double priceUsd;

    double volume24h;

    double holders;

    double liquidity;

    double marketCap;

    double createdAt;

    string creator;
};

class LiquidityParams : public object, public std::enable_shared_from_this<LiquidityParams> {
public:
    using std::enable_shared_from_this<LiquidityParams>::shared_from_this;
    string tokenA;

    string tokenB;

    std::shared_ptr<BigNumberish> amountA;

    std::shared_ptr<BigNumberish> amountB;

    double slippage;

    double deadline;
};

class RemoveLiquidityParams : public object, public std::enable_shared_from_this<RemoveLiquidityParams> {
public:
    using std::enable_shared_from_this<RemoveLiquidityParams>::shared_from_this;
    string lpToken;

    std::shared_ptr<BigNumberish> liquidity;

    std::shared_ptr<BigNumberish> minAmountA;

    std::shared_ptr<BigNumberish> minAmountB;

    double deadline;
};

class LiquidityResult : public object, public std::enable_shared_from_this<LiquidityResult> {
public:
    using std::enable_shared_from_this<LiquidityResult>::shared_from_this;
    any lpTokens;

    string transactionHash;

    std::tuple<any, any> actualAmounts;

    string lpTokenAddress;
};

class SwapParams : public object, public std::enable_shared_from_this<SwapParams> {
public:
    using std::enable_shared_from_this<SwapParams>::shared_from_this;
    string fromToken;

    string toToken;

    std::shared_ptr<BigNumberish> amount;

    double slippage;

    string recipient;

    double deadline;
};

class SwapRoute : public object, public std::enable_shared_from_this<SwapRoute> {
public:
    using std::enable_shared_from_this<SwapRoute>::shared_from_this;
    string token;

    string pool;

    double fee;
};

class SwapResult : public object, public std::enable_shared_from_this<SwapResult> {
public:
    using std::enable_shared_from_this<SwapResult>::shared_from_this;
    any outputAmount;

    string transactionHash;

    array<std::shared_ptr<SwapRoute>> route;

    double priceImpact;

    any gasUsed;
};

class Transaction : public object, public std::enable_shared_from_this<Transaction> {
public:
    using std::enable_shared_from_this<Transaction>::shared_from_this;
    string to;

    string from;

    std::shared_ptr<BigNumberish> value;

    string data;

    std::shared_ptr<BigNumberish> gasLimit;

    std::shared_ptr<BigNumberish> gasPrice;

    double nonce;
};

class SignedTransaction : public object, public std::enable_shared_from_this<SignedTransaction> {
public:
    using std::enable_shared_from_this<SignedTransaction>::shared_from_this;
    string hash;

    string raw;
};

class TransactionReceipt : public object, public std::enable_shared_from_this<TransactionReceipt> {
public:
    using std::enable_shared_from_this<TransactionReceipt>::shared_from_this;
    string hash;

    double blockNumber;

    string blockHash;

    any gasUsed;

    boolean status;

    array<any> logs;
};

class ErrorResponse : public object, public std::enable_shared_from_this<ErrorResponse> {
public:
    using std::enable_shared_from_this<ErrorResponse>::shared_from_this;
    string code;

    string message;

    any details;

    array<string> suggestions;
};

enum struct ErrorCode {
    VALIDATION_ERROR = std:("VALIDATION_ERROR"), NETWORK_ERROR = std:("NETWORK_ERROR"), PROTOCOL_ERROR = std:("PROTOCOL_ERROR"), SECURITY_ERROR = std:("SECURITY_ERROR"), INSUFFICIENT_BALANCE = std:("INSUFFICIENT_BALANCE"), SLIPPAGE_EXCEEDED = std:("SLIPPAGE_EXCEEDED"), TRANSACTION_FAILED = std:("TRANSACTION_FAILED"), UNAUTHORIZED = std:("UNAUTHORIZED")
};
extern any TokenDeploySchema;
extern any SwapSchema;
extern any LiquiditySchema;
class TransactionStatus : public object, public std::enable_shared_from_this<TransactionStatus> {
public:
    using std::enable_shared_from_this<TransactionStatus>::shared_from_this;
    string hash;

    any status;

    double confirmations;

    string error;
};

class TokenBalance : public object, public std::enable_shared_from_this<TokenBalance> {
public:
    using std::enable_shared_from_this<TokenBalance>::shared_from_this;
    string token;

    string symbol;

    double decimals;

    any balance;

    string formattedBalance;

    double priceUsd;

    double valueUsd;
};

extern object POOL_POSITIONS;
extern object FEE_CONFIGS;
#endif
