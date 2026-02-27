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

// Shared types for OTC Desk plugin - for external use only

using QuoteStatus = ;
using PaymentCurrency = std::string;
using ChainType = std::string;

struct QuoteMemory {
    std::string id;
    std::string quoteId;
    std::string entityId;
    std::string beneficiary;
    std::string tokenAmount;
    double discountBps;
    double apr;
    double lockupMonths;
    double lockupDays;
    PaymentCurrency paymentCurrency;
    double priceUsdPerToken;
    double totalUsd;
    double discountUsd;
    double discountedUsd;
    std::string paymentAmount;
    QuoteStatus status;
    std::string signature;
    double createdAt;
    double executedAt;
    double rejectedAt;
    double approvedAt;
    std::string offerId;
    std::string transactionHash;
    double blockNumber;
    std::string rejectionReason;
    std::string approvalNote;
    std::optional<ChainType> chain;
    std::optional<std::string> tokenId;
    std::optional<std::string> tokenSymbol;
    std::optional<std::string> tokenName;
    std::optional<std::string> tokenLogoUrl;
    std::optional<std::string> consignmentId;
};

struct UserSessionMemory {
    std::string id;
    std::string entityId;
    std::string walletAddress;
    double quotesCreated;
    double lastQuoteAt;
    double dailyQuoteCount;
    double dailyResetAt;
    double totalDeals;
    double totalVolumeUsd;
    double totalSavedUsd;
    double createdAt;
    double updatedAt;
};


} // namespace elizaos
