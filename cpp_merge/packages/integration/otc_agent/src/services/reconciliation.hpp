#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_RECONCILIATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTC-AGENT_SRC_SERVICES_RECONCILIATION_H
#include "core.h"
#include "viem.h"
#include "@/contracts/artifacts/contracts/OTC.sol/OTC.json.h"
using otcArtifact = _default;
#include "./database.h"
#include "@/lib/getChain.h"
#include "@/lib/getContractAddress.h"

class SimplePublicClient;
class OnChainOffer;
class ReconciliationService;

class SimplePublicClient : public object, public std::enable_shared_from_this<SimplePublicClient> {
public:
    using std::enable_shared_from_this<SimplePublicClient>::shared_from_this;
    std::function<std::shared_ptr<Promise<any>>(object)> readContract;

    std::function<std::shared_ptr<Promise<any>>()> getBlockNumber;
};

class OnChainOffer : public object, public std::enable_shared_from_this<OnChainOffer> {
public:
    using std::enable_shared_from_this<OnChainOffer>::shared_from_this;
    std::shared_ptr<Address> beneficiary;

    any tokenAmount;

    any discountBps;

    any createdAt;

    any unlockTime;

    any priceUsdPerToken;

    any ethUsdPrice;

    double currency;

    boolean approved;

    boolean paid;

    boolean fulfilled;

    boolean cancelled;

    std::shared_ptr<Address> payer;

    any amountPaid;
};

class ReconciliationService : public object, public std::enable_shared_from_this<ReconciliationService> {
public:
    using std::enable_shared_from_this<ReconciliationService>::shared_from_this;
    std::shared_ptr<SimplePublicClient> client;

    any otcAddress;

    std::shared_ptr<Abi> abi;

    ReconciliationService();
    template <typename P0>
    std::shared_ptr<Promise<std::shared_ptr<OnChainOffer>>> readContractOffer(P0 offerId);
    virtual std::shared_ptr<Promise<object>> reconcileQuote(string quoteId);
    virtual std::shared_ptr<Promise<object>> reconcileAllActive();
    virtual std::shared_ptr<Promise<object>> verifyQuoteState(string quoteId);
    virtual std::shared_ptr<Promise<object>> healthCheck();
};

extern any reconciliationServiceInstance;
std::shared_ptr<ReconciliationService> getReconciliationService();

std::shared_ptr<Promise<void>> runReconciliationTask();

template <typename P0>
std::shared_ptr<Promise<std::shared_ptr<OnChainOffer>>> ReconciliationService::readContractOffer(P0 offerId)
{
    if (!this->otcAddress) throw any(std::make_shared<Error>(std:("OTC address not configured")));
    auto [beneficiary, tokenAmount, discountBps, createdAt, unlockTime, priceUsdPerToken, ethUsdPrice, currency, approved, paid, fulfilled, cancelled, payer, amountPaid] = as<std::tuple<std::shared_ptr<Address>, any, any, any, any, any, any, double, boolean, boolean, boolean, boolean, std::shared_ptr<Address>, any>>((std::async([=]() { this->client->readContract(object{
        object::pair{std:("address"), this->otcAddress}, 
        object::pair{std:("abi"), this->abi}, 
        object::pair{std:("functionName"), std:("offers")}, 
        object::pair{std:("args"), array<any>{ BigInt(offerId) }}
    }); })));
    return object{
        object::pair{std:("beneficiary"), std:("beneficiary")}, 
        object::pair{std:("tokenAmount"), std:("tokenAmount")}, 
        object::pair{std:("discountBps"), std:("discountBps")}, 
        object::pair{std:("createdAt"), std:("createdAt")}, 
        object::pair{std:("unlockTime"), std:("unlockTime")}, 
        object::pair{std:("priceUsdPerToken"), std:("priceUsdPerToken")}, 
        object::pair{std:("ethUsdPrice"), std:("ethUsdPrice")}, 
        object::pair{std:("currency"), std:("currency")}, 
        object::pair{std:("approved"), std:("approved")}, 
        object::pair{std:("paid"), std:("paid")}, 
        object::pair{std:("fulfilled"), std:("fulfilled")}, 
        object::pair{std:("cancelled"), std:("cancelled")}, 
        object::pair{std:("payer"), std:("payer")}, 
        object::pair{std:("amountPaid"), std:("amountPaid")}
    };
}

#endif
