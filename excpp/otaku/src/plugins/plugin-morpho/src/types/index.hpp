#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Type exports for better compatibility

/**
 * Morpho Supply Parameters
 */
struct MorphoSupplyParams {
    std::string asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std::string> onBehalf;
};

/**
 * Morpho Supply Result
 */
struct MorphoSupplyResult {
    std::string transactionHash;
    BigNumber suppliedAmount;
    BigNumber matchedAmount;
    BigNumber poolAmount;
    double improvedAPY;
    double matchingEfficiency;
};

/**
 * Morpho Borrow Parameters
 */
struct MorphoBorrowParams {
    std::string asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std::string> onBehalf;
};

/**
 * Morpho Borrow Result
 */
struct MorphoBorrowResult {
    std::string transactionHash;
    BigNumber borrowedAmount;
    BigNumber matchedAmount;
    BigNumber poolAmount;
    double matchedRate;
    double poolRate;
    double rateImprovement;
};

/**
 * Morpho Withdraw Parameters
 */
struct MorphoWithdrawParams {
    std::string asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std::string> receiver;
};

/**
 * Morpho Withdraw Result
 */
struct MorphoWithdrawResult {
    std::string transactionHash;
    BigNumber withdrawnAmount;
    BigNumber matchingImpact;
    BigNumber fromMatched;
    BigNumber fromPool;
    BigNumber gasUsed;
};

/**
 * Morpho Repay Parameters
 */
struct MorphoRepayParams {
    std::string asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std::string> onBehalf;
};

/**
 * Morpho Repay Result
 */
struct MorphoRepayResult {
    std::string transactionHash;
    BigNumber repaidAmount;
    BigNumber interestSaved;
    BigNumber remainingDebt;
    double newHealthFactor;
};

/**
 * Morpho Position Data
 */
struct MorphoPosition {
    BigNumber totalSupplied;
    BigNumber totalBorrowed;
    std::vector<MorphoAssetPosition> supplies;
    std::vector<MorphoAssetPosition> borrows;
    BigNumber totalMatchedSupply;
    BigNumber totalMatchedBorrow;
    double matchingEfficiency;
    double healthFactor;
};

/**
 * Individual Asset Position
 */
struct MorphoAssetPosition {
    std::string asset;
    std::string symbol;
    BigNumber totalAmount;
    BigNumber matchedAmount;
    BigNumber poolAmount;
    double matchedAPY;
    double poolAPY;
    double averageAPY;
    double matchingRatio;
    std::optional<double> currentAPY;
};

/**
 * Rate Comparison Data
 */
struct RateComparison {
    std::string asset;
    double morphoSupplyAPY;
    double morphoBorrowAPY;
    double poolSupplyAPY;
    double poolBorrowAPY;
    double supplyImprovement;
    double borrowImprovement;
    double matchingPercentage;
    std::optional<std::string> recommendedAction;
};

/**
 * Morpho Market Data
 */
struct MorphoMarketData {
    std::optional<std::string> marketId;
    std::string name;
    BigNumber totalSupply;
    BigNumber totalBorrow;
    double supplyRate;
    double borrowRate;
    double utilizationRate;
    BigNumber liquidity;
    double decimals;
    double lltv;
    double liquidationPenalty;
};

/**
 * Morpho Error Response
 */
struct MorphoErrorResponse {
    std::string code;
    std::string message;
    std::optional<std::any> details;
    std::optional<std::vector<std::string>> suggestions;
    std::optional<MatchingImpact> matchingImpact;
    std::optional<std::vector<std::string>> fallbackOptions;
};

/**
 * Matching Impact Data
 */
struct MatchingImpact {
    double expectedMatching;
    double actualMatching;
    BigNumber gasUsed;
    double rateImpact;
};

/**
 * Plugin Configuration
 */
struct MorphoConfig {
    std::variant<"base", "base-sepolia"> network;
    std::string rpcUrl;
    std::optional<std::string> morphoApiUrl;
    BigNumber defaultMaxGasForMatching;
    double matchingEfficiencyThreshold;
    double rateImprovementThreshold;
    BigNumber maxGasPrice;
    double retryAttempts;
    double monitoringInterval;
};

/**
 * Transaction Options
 */
struct TransactionOptions {
    std::optional<BigNumber> gasLimit;
    std::optional<BigNumber> gasPrice;
    std::optional<BigNumber> maxFeePerGas;
    std::optional<BigNumber> maxPriorityFeePerGas;
    std::optional<double> nonce;
};

/**
 * Service result types
 */
using SupplyResult = MorphoSupplyResult;
using BorrowResult = MorphoBorrowResult;
using WithdrawResult = MorphoWithdrawResult;
using RepayResult = MorphoRepayResult;

/**
 * Action parameter types
 */
struct SupplyActionParams {
    std::string asset;
    std::string amount;
    std::optional<std::string> maxGasForMatching;
};

struct BorrowActionParams {
    std::string asset;
    std::string amount;
    std::optional<std::string> maxGasForMatching;
};

struct WithdrawActionParams {
    std::string asset;
    std::string amount;
    std::optional<std::string> maxGasForMatching;
};

struct RepayActionParams {
    std::string asset;
    std::string amount;
    std::optional<std::string> maxGasForMatching;
};

/**
 * Error codes
 */
enum MorphoErrorCode {
}

/**
 * Asset metadata
 */
struct AssetMetadata {
    std::string address;
    std::string symbol;
    double decimals;
    std::string name;
    bool isActive;
    bool canBeCollateral;
    bool canBeBorrowed;
};

/**
 * Morpho Vault Types
 */
struct MorphoVault {
    std::string address;
    std::string name;
    std::string symbol;
    std::string asset;
    BigNumber totalAssets;
    BigNumber totalSupply;
    double performanceFee;
    double managementFee;
    std::string curator;
    std::string owner;
    std::optional<std::string> pendingOwner;
    std::optional<std::string> guardian;
    std::string feeRecipient;
    std::string skimRecipient;
    double timelock;
    BigNumber lastTotalAssets;
    std::vector<std::string> supplyQueue;
    std::vector<std::string> withdrawQueue;
    double supplyQueueLength;
    double withdrawQueueLength;
};

struct VaultDepositParams {
    std::string vault;
    BigNumber assets;
    std::optional<std::string> receiver;
};

struct VaultDepositResult {
    std::string transactionHash;
    BigNumber shares;
    BigNumber assets;
    BigNumber newBalance;
};

struct VaultWithdrawParams {
    std::string vault;
    std::optional<BigNumber> shares;
    std::optional<BigNumber> assets;
    std::optional<std::string> receiver;
    std::optional<std::string> owner;
};

struct VaultWithdrawResult {
    std::string transactionHash;
    BigNumber shares;
    BigNumber assets;
    BigNumber newBalance;
};

/**
 * Bundler Types
 */
struct BundleAction {
    std::string target;
    Hex callData;
    std::optional<BigNumber> value;
};

struct BundleParams {
    std::vector<BundleAction> actions;
    std::optional<bool> revertOnFailure;
};

struct BundleResult {
    std::string transactionHash;
    std::vector<std::any> results;
    BigNumber gasUsed;
};

/**
 * Rewards Types
 */
struct RewardsClaim {
    std::string token;
    BigNumber amount;
    std::vector<std::string> proof;
};

struct RewardsClaimParams {
    std::vector<RewardsClaim> claims;
    std::optional<std::string> receiver;
};

struct RewardsClaimResult {
    std::string transactionHash;
    BigNumber totalClaimed;
};

struct UserRewards {
    BigNumber totalValue;
    double lastUpdate;
};

/**
 * Liquidation Types
 */
struct LiquidationAlert {
    std::string userAddress;
    double healthFactor;
    std::variant<"LOW", "MEDIUM", "HIGH", "CRITICAL"> riskLevel;
    std::vector<std::string> recommendedActions;
    std::optional<double> timeToLiquidation;
};

struct PreLiquidationParams {
    std::string borrower;
    std::string repayAsset;
    std::string seizeAsset;
    std::optional<BigNumber> repayAmount;
};

/**
 * Oracle Types
 */
struct PriceFeed {
    std::string asset;
    BigNumber price;
    double decimals;
    double updatedAt;
    std::string source;
};

struct HealthFactorData {
    double healthFactor;
    double liquidationThreshold;
    double maxLtv;
    BigNumber totalCollateralValue;
    BigNumber totalDebtValue;
    BigNumber availableBorrowingPower;
};

/**
 * Market Creation Types
 */
struct MarketCreationParams {
    std::string loanToken;
    std::string collateralToken;
    std::string oracle;
    std::string irm;
    BigNumber lltv;
};

struct MarketCreationResult {
    Hex marketId;
    std::string transactionHash;
    std::string loanToken;
    std::string collateralToken;
    std::string oracle;
    std::string irm;
    BigNumber lltv;
};

/**
 * Public Allocator Types
 */
struct AllocationConfig {
    std::string vault;
    Hex marketId;
    BigNumber maxIn;
    BigNumber maxOut;
};

struct ReallocateParams {
    std::string vault;
    Hex marketId;
    BigNumber assets;
};

/**
 * Enhanced Action Parameter Types
 */
struct VaultDepositActionParams {
    std::string vault;
    std::string amount;
    std::optional<std::string> receiver;
};

struct VaultWithdrawActionParams {
    std::string vault;
    std::string amount;
    std::optional<bool> isShares;
    std::optional<std::string> receiver;
};

struct BundledSupplyBorrowParams {
    std::string supplyAsset;
    std::string supplyAmount;
    std::string borrowAsset;
    std::string borrowAmount;
    std::optional<std::string> maxGasForMatching;
};

struct ClaimRewardsActionParams {
    std::optional<std::vector<std::string>> tokens;
    std::optional<std::string> receiver;
};

using MorphoApiMarket = {

using MarketSummary = {

struct UserPosition {
    std::string marketId;
    std::string pairLabel;
    std::string collateral;
    std::string loan;
    double collateral;
    double loan;
    std::string collateralTokens;
    std::string loanTokens;
    std::string collateralUsd;
    std::string loanUsd;
    std::string suppliedTokens;
    std::string suppliedUsd;
    std::string withdrawableTokens;
    std::string borrowShares;
    std::string supplyShares;
    std::optional<double> collateralUsd;
    std::optional<double> loanUsd;
    std::string liquidationLoanPerCollateral;
    std::string currentLoanPerCollateral;
    double lltvPct;
    std::optional<double> ltvPct;
    std::optional<double> dropToLiquidationPct;
    bool hasSupplied;
    std::string earnedInterest;
    std::optional<double> currentApy;
    bool hasPosition;
};

using UserVaultPosition = {

using MorphoVaultData = {
  // Totals

  // APYs (decimals, e.g. 0.046 -> 4.6%)

  // Optional: per-allocation info (array of markets)

} // namespace elizaos
