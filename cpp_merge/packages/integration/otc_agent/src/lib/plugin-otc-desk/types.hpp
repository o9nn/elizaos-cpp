#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_PLUGIN-OTC-DESK_TYPES_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_LIB_PLUGIN-OTC-DESK_TYPES_H
#include "core.h"

typedef any QuoteStatus;

typedef any PaymentCurrency;

typedef any ChainType;

class QuoteMemory;
class UserSessionMemory;

class QuoteMemory : public object, public std::enable_shared_from_this<QuoteMemory> {
public:
    using std::enable_shared_from_this<QuoteMemory>::shared_from_this;
    string id;

    string quoteId;

    string entityId;

    string beneficiary;

    string tokenAmount;

    double discountBps;

    double apr;

    double lockupMonths;

    double lockupDays;

    PaymentCurrency paymentCurrency;

    double priceUsdPerToken;

    double totalUsd;

    double discountUsd;

    double discountedUsd;

    string paymentAmount;

    QuoteStatus status;

    string signature;

    double createdAt;

    double executedAt;

    double rejectedAt;

    double approvedAt;

    string offerId;

    string transactionHash;

    double blockNumber;

    string rejectionReason;

    string approvalNote;

    ChainType chain;

    string tokenId;

    string tokenSymbol;

    string tokenName;

    string tokenLogoUrl;

    string consignmentId;
};

class UserSessionMemory : public object, public std::enable_shared_from_this<UserSessionMemory> {
public:
    using std::enable_shared_from_this<UserSessionMemory>::shared_from_this;
    string id;

    string entityId;

    string walletAddress;

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

#endif
