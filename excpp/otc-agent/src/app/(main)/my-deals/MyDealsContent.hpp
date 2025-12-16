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



// Type for deals from the API
struct DealFromAPI {
    std::string offerId;
    std::string beneficiary;
    std::string tokenAmount;
    double discountBps;
    std::string paymentCurrency;
    std::string paymentAmount;
    std::string payer;
    std::string createdAt;
    std::optional<double> lockupMonths;
    std::optional<double> lockupDays;
    std::optional<std::string> quoteId;
    std::optional<std::string> status;
    std::optional<std::string> tokenSymbol;
    std::optional<std::string> tokenName;
    std::optional<std::string> tokenLogoUrl;
    std::optional<std::string> tokenId;
    std::optional<std::string> chain;
    std::optional<double> priceUsdPerToken;
    std::optional<double> ethUsdPrice;
    std::optional<double> totalUsd;
    std::optional<double> discountedUsd;
};

// Extended offer type with quoteId and token metadata
struct OfferWithQuoteId {
    bigint id;
    std::string beneficiary;
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
    std::string payer;
    bigint amountPaid;
    std::optional<std::string> quoteId;
    std::optional<std::string> tokenSymbol;
    std::optional<std::string> tokenName;
    std::optional<std::string> tokenLogoUrl;
    std::optional<std::string> tokenId;
    std::optional<std::string> chain;
};

std::string formatDate(bigint tsSeconds);

std::string formatTokenAmount(bigint amount);

std::string getLockupLabel(bigint createdAt, bigint unlockTime);

// --- Helper: Transform Solana deals from API to offer format ---
OfferWithQuoteId transformSolanaDeal(DealFromAPI deal, const std::string& walletAddress);

// --- Helper: Transform EVM deal from API to offer format ---
OfferWithQuoteId transformEvmDeal(DealFromAPI deal, const std::string& walletAddress);

// --- Helper: Merge database deals with contract offers ---
std::vector<OfferWithQuoteId> mergeDealsWithOffers(const std::vector<DealFromAPI>& dbDeals, const std::vector<OfferWithQuoteId>& contractOffers, const std::string& walletAddress);

void MyDealsContent();

} // namespace elizaos
