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
 * Consolidated Type Definitions
 * Single source of truth for all shared types across the OTC Agent
 */

// Re-from specific type files

// Re-from plugin types

//========
// CHAIN TYPES
//========

using EVMChain = std::variant<"base", "bsc">;

//========
// OTC CONTRACT TYPES
//========

/**
 * OTC Offer structure (matches Solidity contract)
 */
struct Offer {
    bigint consignmentId;
    std:; // bytes32 hex std: tokenId;
    Address beneficiary;
    bigint tokenAmount;
    bigint discountBps;
    bigint createdAt;
    bigint unlockTime;
    bigint; // 8 decimals priceUsdPerToken;
    bigint maxPriceDeviation;
    bigint; // 8 decimals ethUsdPrice;
    number; // 0 = ETH, 1 = USDC currency;
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
    std: tokenId;
    std: tokenSymbol;
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
    std: quoteId;
    std: tokenSymbol;
    double tokenAmount;
    std: tokenChain;
    double discountBps;
    double discountPercent;
    double lockupDays;
    double lockupMonths;
    PaymentCurrency paymentCurrency;
    double apr;
    double totalUsd;
    double discountedUsd;
    std: paymentAmount;
    std::optional<std:> signature;
    std::optional<bool> isFixedPrice;
};

/**
 * Quote accepted message
 */
struct QuoteAccepted {
    std: quoteId;
    std: txHash;
};

//========
// DATABASE TYPES
//========

/**
 * Token in database
 */
struct Token {
    std: id;
    std: symbol;
    std: name;
    std: contractAddress;
    Chain chain;
    double decimals;
    std: logoUrl;
    std: description;
    std::optional<std:> website;
    std::optional<std:> twitter;
    bool isActive;
    double createdAt;
    double updatedAt;
};

/**
 * Token market data
 */
struct TokenMarketData {
    std: tokenId;
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
    std: id;
    std: tokenId;
    std: consignerAddress;
    std: consignerEntityId;
    std: totalAmount;
    std: remainingAmount;
    bool isNegotiable;
    std::optional<double> fixedDiscountBps;
    std::optional<double> fixedLockupDays;
    double minDiscountBps;
    double maxDiscountBps;
    double minLockupDays;
    double maxLockupDays;
    std: minDealAmount;
    std: maxDealAmount;
    bool isFractionalized;
    bool isPrivate;
    std::optional<std::vector<std::string>> allowedBuyers;
    double maxPriceVolatilityBps;
    double maxTimeToExecuteSeconds;
    "active" | "paused" | "depleted" | "withdrawn" status;
    std::optional<std:> contractConsignmentId;
    Chain chain;
    double createdAt;
    double updatedAt;
    std::optional<double> lastDealAt;
};

/**
 * Consignment deal record
 */
struct ConsignmentDeal {
    std: id;
    std: consignmentId;
    std: quoteId;
    std: tokenId;
    std: buyerAddress;
    std: amount;
    double discountBps;
    double lockupDays;
    double executedAt;
    std::optional<std:> offerId;
    "pending" | "executed" | "failed" status;
};

//========
// USER SESSION TYPES
//========

/**
 * User session memory
 */
struct UserSessionMemory {
    std: id;
    std: entityId;
    std: walletAddress;
    "evm" | "solana" chainFamily;
    std::optional<std:> preferredChain;
    double lastActiveAt;
    std::optional<std::unordered_map<std:, unknown>> sessionData;
    double createdAt;
    double updatedAt;
};

//========
// UTILITY TYPES
//========

/**
 * Token with balance information
 */

/**
 * Consignment creation result
 */
struct ConsignmentCreationResult {
    "0x" + std::to_string(string) + "" txHash;


} // namespace elizaos
