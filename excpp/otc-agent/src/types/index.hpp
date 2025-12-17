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
 * Consolidated Type Definitions
 * Single source of truth for all shared types across the OTC Agent
 */

// Re-from specific type files

// Re-from plugin types

// CHAIN TYPES

using EVMChain = std::string;

// OTC CONTRACT TYPES

/**
 * OTC Offer structure (matches Solidity contract)
 */
struct Offer {
    bigint consignmentId;
    std::string tokenId;
    Address beneficiary;
    bigint tokenAmount;
    bigint discountBps;
    bigint createdAt;
    bigint unlockTime;
    bigint priceUsdPerToken;
    bigint maxPriceDeviation;
    bigint ethUsdPrice;
    double currency;
    bool approved;
    bool paid;
    bool fulfilled;
    bool cancelled;
    Address payer;
    bigint amountPaid;
};

/**
 * Consignment parameters for on-chain creation
 */
struct ConsignmentParams {
    std::string tokenId;
    std::string tokenSymbol;
    bigint amount;
    bool isNegotiable;
    double fixedDiscountBps;
    double fixedLockupDays;
    double minDiscountBps;
    double maxDiscountBps;
    double minLockupDays;
    double maxLockupDays;
    bigint minDealAmount;
    bigint maxDealAmount;
    bool isFractionalized;
    bool isPrivate;
    double maxPriceVolatilityBps;
    double maxTimeToExecute;
    bigint gasDeposit;
};

/**
 * OTC Quote for XML parsing and frontend display
 */
struct OTCQuote {
    std::string quoteId;
    std::string tokenSymbol;
    double tokenAmount;
    std::string tokenChain;
    double discountBps;
    double discountPercent;
    double lockupDays;
    double lockupMonths;
    PaymentCurrency paymentCurrency;
    double apr;
    double totalUsd;
    double discountedUsd;
    std::string paymentAmount;
    std::optional<std::string> signature;
    std::optional<bool> isFixedPrice;
};

/**
 * Quote accepted message
 */
struct QuoteAccepted {
    std::string quoteId;
    std::string txHash;
};

// DATABASE TYPES

/**
 * Token in database
 */
struct Token {
    std::string id;
    std::string symbol;
    std::string name;
    std::string contractAddress;
    Chain chain;
    double decimals;
    std::string logoUrl;
    std::string description;
    std::optional<std::string> website;
    std::optional<std::string> twitter;
    bool isActive;
    double createdAt;
    double updatedAt;
};

/**
 * Token market data
 */
struct TokenMarketData {
    std::string tokenId;
    double priceUsd;
    double marketCap;
    double volume24h;
    double priceChange24h;
    double liquidity;
    double lastUpdated;
};

/**
 * OTC Consignment in database
 */
struct OTCConsignment {
    std::string id;
    std::string tokenId;
    std::string consignerAddress;
    std::string consignerEntityId;
    std::string totalAmount;
    std::string remainingAmount;
    bool isNegotiable;
    std::optional<double> fixedDiscountBps;
    std::optional<double> fixedLockupDays;
    double minDiscountBps;
    double maxDiscountBps;
    double minLockupDays;
    double maxLockupDays;
    std::string minDealAmount;
    std::string maxDealAmount;
    bool isFractionalized;
    bool isPrivate;
    std::optional<std::vector<std::string>> allowedBuyers;
    double maxPriceVolatilityBps;
    double maxTimeToExecuteSeconds;
    std::string status;
    std::optional<std::string> contractConsignmentId;
    Chain chain;
    double createdAt;
    double updatedAt;
    std::optional<double> lastDealAt;
};

/**
 * Consignment deal record
 */
struct ConsignmentDeal {
    std::string id;
    std::string consignmentId;
    std::string quoteId;
    std::string tokenId;
    std::string buyerAddress;
    std::string amount;
    double discountBps;
    double lockupDays;
    double executedAt;
    std::optional<std::string> offerId;
    std::string status;
};

// USER SESSION TYPES

/**
 * User session memory
 */
struct UserSessionMemory {
    std::string id;
    std::string entityId;
    std::string walletAddress;
    std::string chainFamily;
    std::optional<std::string> preferredChain;
    double lastActiveAt;
    double createdAt;
    double updatedAt;
};

// UTILITY TYPES

/**
 * Token with balance information
 */

/**
 * Consignment creation result
 */
struct ConsignmentCreationResult {
    bigint consignmentId;
};


} // namespace elizaos
