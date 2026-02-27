#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_TYPES_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_TYPES_INDEX_H
#include "core.h"
#include "viem.h"
#include "@/config/chains.h"
#include "@/lib/plugin-otc-desk/types.h"

typedef any EVMChain;

class Offer;
class ConsignmentParams;
class OTCQuote;
class QuoteAccepted;
class Token;
class TokenMarketData;
class OTCConsignment;
class ConsignmentDeal;
class UserSessionMemory;
class TokenWithBalance;
class ConsignmentCreationResult;

class Offer : public object, public std::enable_shared_from_this<Offer> {
public:
    using std::enable_shared_from_this<Offer>::shared_from_this;
    any consignmentId;

    string tokenId;

    std::shared_ptr<Address> beneficiary;

    any tokenAmount;

    any discountBps;

    any createdAt;

    any unlockTime;

    any priceUsdPerToken;

    any maxPriceDeviation;

    any ethUsdPrice;

    double currency;

    boolean approved;

    boolean paid;

    boolean fulfilled;

    boolean cancelled;

    std::shared_ptr<Address> payer;

    any amountPaid;
};

class ConsignmentParams : public object, public std::enable_shared_from_this<ConsignmentParams> {
public:
    using std::enable_shared_from_this<ConsignmentParams>::shared_from_this;
    string tokenId;

    string tokenSymbol;

    any amount;

    boolean isNegotiable;

    double fixedDiscountBps;

    double fixedLockupDays;

    double minDiscountBps;

    double maxDiscountBps;

    double minLockupDays;

    double maxLockupDays;

    any minDealAmount;

    any maxDealAmount;

    boolean isFractionalized;

    boolean isPrivate;

    double maxPriceVolatilityBps;

    double maxTimeToExecute;

    any gasDeposit;
};

class OTCQuote : public object, public std::enable_shared_from_this<OTCQuote> {
public:
    using std::enable_shared_from_this<OTCQuote>::shared_from_this;
    string quoteId;

    string tokenSymbol;

    double tokenAmount;

    string tokenChain;

    double discountBps;

    double discountPercent;

    double lockupDays;

    double lockupMonths;

    std::shared_ptr<PaymentCurrency> paymentCurrency;

    double apr;

    double totalUsd;

    double discountedUsd;

    string paymentAmount;

    string signature;

    boolean isFixedPrice;
};

class QuoteAccepted : public object, public std::enable_shared_from_this<QuoteAccepted> {
public:
    using std::enable_shared_from_this<QuoteAccepted>::shared_from_this;
    string quoteId;

    string txHash;
};

class Token : public object, public std::enable_shared_from_this<Token> {
public:
    using std::enable_shared_from_this<Token>::shared_from_this;
    string id;

    string symbol;

    string name;

    string contractAddress;

    std::shared_ptr<Chain> chain;

    double decimals;

    string logoUrl;

    string description;

    string website;

    string twitter;

    boolean isActive;

    double createdAt;

    double updatedAt;
};

class TokenMarketData : public object, public std::enable_shared_from_this<TokenMarketData> {
public:
    using std::enable_shared_from_this<TokenMarketData>::shared_from_this;
    string tokenId;

    double priceUsd;

    double marketCap;

    double volume24h;

    double priceChange24h;

    double liquidity;

    double lastUpdated;
};

class OTCConsignment : public object, public std::enable_shared_from_this<OTCConsignment> {
public:
    using std::enable_shared_from_this<OTCConsignment>::shared_from_this;
    string id;

    string tokenId;

    string consignerAddress;

    string consignerEntityId;

    string totalAmount;

    string remainingAmount;

    boolean isNegotiable;

    double fixedDiscountBps;

    double fixedLockupDays;

    double minDiscountBps;

    double maxDiscountBps;

    double minLockupDays;

    double maxLockupDays;

    string minDealAmount;

    string maxDealAmount;

    boolean isFractionalized;

    boolean isPrivate;

    array<string> allowedBuyers;

    double maxPriceVolatilityBps;

    double maxTimeToExecuteSeconds;

    any status;

    string contractConsignmentId;

    std::shared_ptr<Chain> chain;

    double createdAt;

    double updatedAt;

    double lastDealAt;
};

class ConsignmentDeal : public object, public std::enable_shared_from_this<ConsignmentDeal> {
public:
    using std::enable_shared_from_this<ConsignmentDeal>::shared_from_this;
    string id;

    string consignmentId;

    string quoteId;

    string tokenId;

    string buyerAddress;

    string amount;

    double discountBps;

    double lockupDays;

    double executedAt;

    string offerId;

    any status;
};

class UserSessionMemory : public object, public std::enable_shared_from_this<UserSessionMemory> {
public:
    using std::enable_shared_from_this<UserSessionMemory>::shared_from_this;
    string id;

    string entityId;

    string walletAddress;

    any chainFamily;

    string preferredChain;

    double lastActiveAt;

    Record<string, any> sessionData;

    double createdAt;

    double updatedAt;
};

class TokenWithBalance : public Token, public std::enable_shared_from_this<TokenWithBalance> {
public:
    using std::enable_shared_from_this<TokenWithBalance>::shared_from_this;
    string balance;

    string balanceFormatted;

    double balanceUsd;

    double priceUsd;
};

class ConsignmentCreationResult : public object, public std::enable_shared_from_this<ConsignmentCreationResult> {
public:
    using std::enable_shared_from_this<ConsignmentCreationResult>::shared_from_this;
    std::shared_ptr<> txHash;
};

#endif
