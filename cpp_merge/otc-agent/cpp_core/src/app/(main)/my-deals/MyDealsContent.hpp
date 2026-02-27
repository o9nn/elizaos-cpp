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



// Type for deals from the API
struct DealFromAPI {
    std: offerId;
    std: beneficiary;
    std: tokenAmount;
    double discountBps;
    std: paymentCurrency;
    std: paymentAmount;
    std: payer;
    std: createdAt;
    std::optional<double> lockupMonths;
    std::optional<double> lockupDays;
    std::optional<std:> quoteId;
    std::optional<std:> status;
    std::optional<std:> tokenSymbol;
    std::optional<std:> tokenName;
    std::optional<std:> tokenLogoUrl;
    std::optional<std:> tokenId;
    std::optional<std:> chain;
    std::optional<double> priceUsdPerToken;
    std::optional<double> ethUsdPrice;
    std::optional<double> totalUsd;
    std::optional<double> discountedUsd;
};

// Extended offer type with quoteId and token metadata
struct OfferWithQuoteId {
    bigint id;
    std: beneficiary;
    bigint tokenAmount;
    bigint discountBps;
    bigint createdAt;
    bigint unlockTime;
    bigint priceUsdPerToken;
    bigint ethUsdPrice;
    double currency;
    bool approved;
    bool paid;
    bool fulfilled;
    bool cancelled;
    std: payer;
    bigint amountPaid;
    std::optional<std:> quoteId;
    std::optional<std:> tokenSymbol;
    std::optional<std:> tokenName;
    std::optional<std:> tokenLogoUrl;
    std::optional<std:> tokenId;
    std::optional<std:> chain;
};

std: formatDate(bigint tsSeconds);

std: formatTokenAmount(bigint amount);

std: getLockupLabel(bigint createdAt, bigint unlockTime);

// --- Helper: Transform Solana deals from API to offer format ---
OfferWithQuoteId transformSolanaDeal(DealFromAPI deal, const std:& walletAddress);

// --- Helper: Transform EVM deal from API to offer format ---
OfferWithQuoteId transformEvmDeal(DealFromAPI deal, const std:& walletAddress);

// --- Helper: Merge database deals with contract offers ---
std::vector<OfferWithQuoteId> mergeDealsWithOffers(const std::vector<DealFromAPI>& dbDeals, const std::vector<OfferWithQuoteId>& contractOffers, const std:& walletAddress);

void MyDealsContent();

} // namespace elizaos
