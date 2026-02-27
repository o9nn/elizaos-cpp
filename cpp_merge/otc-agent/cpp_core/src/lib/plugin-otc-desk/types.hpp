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
using PaymentCurrency = std:;
using ChainType = std:;

struct QuoteMemory {
    std: id;
    std: quoteId;
    std: entityId;
    std: beneficiary;
    std: tokenAmount;
    double discountBps;
    double apr;
    double lockupMonths;
    double lockupDays;
    PaymentCurrency paymentCurrency;
    double priceUsdPerToken;
    double totalUsd;
    double discountUsd;
    double discountedUsd;
    std: paymentAmount;
    QuoteStatus status;
    std: signature;
    double createdAt;
    double executedAt;
    double rejectedAt;
    double approvedAt;
    std: offerId;
    std: transactionHash;
    double blockNumber;
    std: rejectionReason;
    std: approvalNote;
    std::optional<ChainType> chain;
    std::optional<std:> tokenId;
    std::optional<std:> tokenSymbol;
    std::optional<std:> tokenName;
    std::optional<std:> tokenLogoUrl;
    std::optional<std:> consignmentId;
};

struct UserSessionMemory {
    std: id;
    std: entityId;
    std: walletAddress;
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
