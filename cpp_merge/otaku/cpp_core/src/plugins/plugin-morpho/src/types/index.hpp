#include "elizaos/core.hpp"
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



// Type exports for better compatibility

/**
 * Morpho Supply Parameters
 */
struct MorphoSupplyParams {
    std: asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std:> onBehalf;
};

/**
 * Morpho Supply Result
 */
struct MorphoSupplyResult {
    std: transactionHash;
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
    std: asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std:> onBehalf;
};

/**
 * Morpho Borrow Result
 */
struct MorphoBorrowResult {
    std: transactionHash;
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
    std: asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std:> receiver;
};

/**
 * Morpho Withdraw Result
 */
struct MorphoWithdrawResult {
    std: transactionHash;
    BigNumber withdrawnAmount;
    BigNumber matchingImpact;
    { executionDetails;
    BigNumber fromMatched;
    BigNumber fromPool;
    BigNumber gasUsed;
};

/**
 * Morpho Repay Parameters
 */
struct MorphoRepayParams {
    std: asset;
    BigNumber amount;
    BigNumber maxGasForMatching;
    std::optional<std:> onBehalf;
};

/**
 * Morpho Repay Result
 */
struct MorphoRepayResult {
    std: transactionHash;
    BigNumber repaidAmount;
    BigNumber interestSaved;
    { positionUpdate;
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
    std: asset;
    std: symbol;
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
    std: asset;
    double morphoSupplyAPY;
    double morphoBorrowAPY;
    double poolSupplyAPY;
    double poolBorrowAPY;
    double supplyImprovement;
    double borrowImprovement;
    double matchingPercentage;
    std::optional<std:> recommendedAction;
};

/**
 * Morpho Market Data
 */
struct MorphoMarketData {
    std::optional<std:> marketId;
    std: name;
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
    std: code;
    std: message;
    std::optional<std:> details;
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
    "base" | "base-sepolia" network;
    std: rpcUrl;
    std::optional<std:> morphoApiUrl;
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
    std: asset;
    std: amount;
    std::optional<std:> maxGasForMatching;
};

struct BorrowActionParams {
    std: asset;
    std: amount;
    std::optional<std:> maxGasForMatching;
};

struct WithdrawActionParams {
    std: asset;
    std: amount;
    std::optional<std:> maxGasForMatching;
};

struct RepayActionParams {
    std: asset;
    std: amount;
    std::optional<std:> maxGasForMatching;
};

/**
 * Error codes
 */
enum MorphoErrorCode {
  INSUFFICIENT_COLLATERAL = "INSUFFICIENT_COLLATERAL",
  MATCHING_FAILED = "MATCHING_FAILED",
  POSITION_NOT_FOUND = "POSITION_NOT_FOUND",
  RATE_CALCULATION_ERROR = "RATE_CALCULATION_ERROR",
  LIQUIDITY_ERROR = "LIQUIDITY_ERROR",
  GAS_ESTIMATION_ERROR = "GAS_ESTIMATION_ERROR",
  TRANSACTION_FAILED = "TRANSACTION_FAILED",
  INVALID_PARAMETERS = "INVALID_PARAMETERS",
  NETWORK_ERROR = "NETWORK_ERROR",
  UNKNOWN_ERROR = "UNKNOWN_ERROR",
}

/**
 * Asset metadata
 */
struct AssetMetadata {
    std: address;
    std: symbol;
    double decimals;
    std: name;
    bool isActive;
    bool canBeCollateral;
    bool canBeBorrowed;
};

/**
 * Morpho Vault Types
 */
struct MorphoVault {
    std: address;
    std: name;
    std: symbol;
    std: asset;
    BigNumber totalAssets;
    BigNumber totalSupply;
    double performanceFee;
    double managementFee;
    std: curator;
    std: owner;
    std::optional<std:> pendingOwner;
    std::optional<std:> guardian;
    std: feeRecipient;
    std: skimRecipient;
    double timelock;
    BigNumber lastTotalAssets;
    std::vector<std::string> supplyQueue;
    std::vector<std::string> withdrawQueue;
    double supplyQueueLength;
    double withdrawQueueLength;
};

struct VaultDepositParams {
    std: vault;
    BigNumber assets;
    std::optional<std:> receiver;
};

struct VaultDepositResult {
    std: transactionHash;
    BigNumber shares;
    BigNumber assets;
    BigNumber newBalance;
};

struct VaultWithdrawParams {
    std: vault;
    std::optional<BigNumber> shares;
    std::optional<BigNumber> assets;
    std::optional<std:> receiver;
    std::optional<std:> owner;
};

struct VaultWithdrawResult {
    std: transactionHash;
    BigNumber shares;
    BigNumber assets;
    BigNumber newBalance;
};

/**
 * Bundler Types
 */
struct BundleAction {
    std: target;
    Hex callData;
    std::optional<BigNumber> value;
};

struct BundleParams {
    std::vector<BundleAction> actions;
    std::optional<bool> revertOnFailure;
};

struct BundleResult {
    std: transactionHash;
    std::vector<std::string> results;
    BigNumber gasUsed;
};

/**
 * Rewards Types
 */
struct RewardsClaim {
    std: token;
    BigNumber amount;
    std::vector<std::string> proof;
};

struct RewardsClaimParams {
    std::vector<RewardsClaim> claims;
    std::optional<std:> receiver;
};

struct RewardsClaimResult {
    std: transactionHash;
    BigNumber totalClaimed;
    std::vector<{ token: std:; amount: BigNumber }> claimedTokens;

struct UserRewards {
    std::vector<{ token: std:; amount: BigNumber }> claimable;

/**
 * Liquidation Types
 */
struct LiquidationAlert {
    std: userAddress;
    double healthFactor;
    "LOW" | "MEDIUM" | "HIGH" | "CRITICAL" riskLevel;
    std::vector<std::string> recommendedActions;
    std::optional<double> timeToLiquidation;
};

struct PreLiquidationParams {
    std: borrower;
    std: repayAsset;
    std: seizeAsset;
    std::optional<BigNumber> repayAmount;
};

/**
 * Oracle Types
 */
struct PriceFeed {
    std: asset;
    BigNumber price;
    double decimals;
    double updatedAt;
    std: source;
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
    std: loanToken;
    std: collateralToken;
    std: oracle;
    std: irm;
    BigNumber; // Loan-to-value ratio in basis points lltv;
};

struct MarketCreationResult {
    Hex marketId;
    std: transactionHash;
    { market;
    std: loanToken;
    std: collateralToken;
    std: oracle;
    std: irm;
    BigNumber lltv;
};

/**
 * Public Allocator Types
 */
struct AllocationConfig {
    std: vault;
    Hex marketId;
    BigNumber maxIn;
    BigNumber maxOut;
};

struct ReallocateParams {
    std: vault;
    Hex marketId;
    BigNumber assets;
};

/**
 * Enhanced Action Parameter Types
 */
struct VaultDepositActionParams {
    std: vault;
    std: amount;
    std::optional<std:> receiver;
};

struct VaultWithdrawActionParams {
    std: vault;
    std: amount;
    std::optional<bool> isShares;
    std::optional<std:> receiver;
};

struct BundledSupplyBorrowParams {
    std: supplyAsset;
    std: supplyAmount;
    std: borrowAsset;
    std: borrowAmount;
    std::optional<std:> maxGasForMatching;
};

struct ClaimRewardsActionParams {
    std::optional<std::vector<std::string>> tokens;
    std::optional<std:> receiver;
};

using MorphoApiMarket = {

using MarketSummary = {

struct UserPosition {
    std: marketId;
    std: pairLabel;
    { symbols;
    std: collateral;
    std: loan;
    { decimals;
    double collateral;
    double loan;
    { amounts;
    std: collateralTokens;
    std: loanTokens;
    std: | null collateralUsd;
    std: | null loanUsd;
    std: suppliedTokens;
    std: | null suppliedUsd;
    std: withdrawableTokens;
    { shares;
    std: borrowShares;
    std: supplyShares;
    { prices;
    number | null collateralUsd;
    number | null loanUsd;
    std: | null liquidationLoanPerCollateral;
    std: | null currentLoanPerCollateral;
    { risk;
    double lltvPct;
    number | null ltvPct;
    number | null dropToLiquidationPct;
    { addresses;
    "0x" + std::to_string(string) + "" collateral;
    "0x" + std::to_string(string) + "" loan;
    "0x" + std::to_string(string) + "" user;
    { supply;
    bool hasSupplied;
    std: | null earnedInterest;
    number | null currentApy;
    bool hasPosition;
};

using UserVaultPosition = {
  vault: {
    address: "0x" + std::to_string(string) + "";
    name: std:;
    asset: {
      address: "0x" + std::to_string(string) + "";
      symbol: std:;
      decimals;
    };
    state: {
      dailyApy | null;
      weeklyApy | null;
      monthlyApy | null;
      yearlyApy | null;
    };
  };
  shares: std:;
  assets: std:;
};

using MorphoVaultData = {
  address: "0x" + std::to_string(string) + "";
  name: std:;
  asset: {
    address: "0x" + std::to_string(string) + "";
    symbol: std:;
    decimals;
  };
  // Totals
  totalDepositsTokens: BigNumber; // state.totalAssets (normalized)
  totalDepositsUsd?: BigNumber | null; // state.totalAssetsUsd (if you want it)
  totalSupplyShares?: BigNumber | null; // state.totalSupply (vault shares)

  // APYs (decimals, e.g. 0.046 -> 4.6%)
  apy: {
    daily | null;
    weekly | null;
    monthly | null;
    yearly | null;
    apy? | null; // overall apy if you want to expose it
  };

  // Optional: per-allocation info (array of markets)
  allocations?: Array<{
    marketId: std:;
    supplyAssetsTokens: BigNumber;
    supplyAssetsUsd?: BigNumber | null;
    supplyCapTokens?: BigNumber | null;
  }>;
};

} // namespace elizaos
