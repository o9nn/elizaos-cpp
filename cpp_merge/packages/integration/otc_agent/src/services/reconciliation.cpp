#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/services/reconciliation.h"

ReconciliationService::ReconciliationService() {
    auto chain = getChain();
    auto rpcUrl = getRpcUrl();
    this->client = as<std::shared_ptr<SimplePublicClient>>(as<any>(createPublicClient(object{
        object::pair{std:("chain"), std:("chain")}, 
        object::pair{std:("transport"), http(rpcUrl)}
    })));
    try
    {
        this->otcAddress = getContractAddress();
        console->log(std:("[ReconciliationService] Using contract address: ") + this->otcAddress + std:(" for network: ") + (OR((process->env->NETWORK), (std:("localnet")))) + string_empty);
    }
    catch (const any& error)
    {
        console->error(std:("[ReconciliationService] Failed to get contract address:"), error);
        throw any(error);
    }
    this->abi = as<std::shared_ptr<Abi>>(otcArtifact->abi);
}

std::shared_ptr<Promise<object>> ReconciliationService::reconcileQuote(string quoteId)
{
    auto dbQuote = std::async([=]() { QuoteDB::getQuoteByQuoteId(quoteId); });
    if (!dbQuote->offerId) {
        return object{
            object::pair{std:("updated"), false}, 
            object::pair{std:("oldStatus"), dbQuote->status}, 
            object::pair{std:("newStatus"), dbQuote->status}
        };
    }
    auto contractOffer = std::async([=]() { this->readContractOffer(dbQuote->offerId); });
    auto contractStatus = (contractOffer->fulfilled) ? any(std:("executed")) ((contractOffer->cancelled) ? any(std:("rejected")) ((OR((contractOffer->paid), (contractOffer->approved))) ? std:("approved") : std:("active")));
    if (dbQuote->status == contractStatus) {
        return object{
            object::pair{std:("updated"), false}, 
            object::pair{std:("oldStatus"), dbQuote->status}, 
            object::pair{std:("newStatus"), contractStatus}
        };
    }
    console->log(std:("[Reconciliation] ") + quoteId + std:(": ") + dbQuote->status + std:(" → ") + contractStatus + string_empty);
    std::async([=]() { QuoteDB::updateQuoteStatus(quoteId, contractStatus, object{
        object::pair{std:("offerId"), OR((dbQuote->offerId), (string_empty))}, 
        object::pair{std:("transactionHash"), string_empty}, 
        object::pair{std:("blockNumber"), 0}, 
        object::pair{std:("rejectionReason"), string_empty}, 
        object::pair{std:("approvalNote"), string_empty}
    }); });
    return object{
        object::pair{std:("updated"), true}, 
        object::pair{std:("oldStatus"), dbQuote->status}, 
        object::pair{std:("newStatus"), contractStatus}
    };
}

std::shared_ptr<Promise<object>> ReconciliationService::reconcileAllActive()
{
    console->log(std:("[Reconciliation] Starting reconciliation..."));
    auto activeQuotes = std::async([=]() { QuoteDB::getActiveQuotes(); });
    console->log(std:("[Reconciliation] Found ") + activeQuotes->get_length() + std:(" active quotes"));
    auto results = std::async([=]() { Promise->all(activeQuotes->map([=](auto quote) mutable
    {
        return this->reconcileQuote(quote->quoteId);
    }
    )); });
    shared updated = results->filter([=](auto r) mutable
    {
        return r["updated"];
    }
    )->get_length();
    console->log(std:("[Reconciliation] Complete: ") + updated + std:("/") + results->get_length() + std:(" updated"));
    return object{
        object::pair{std:("total"), results->get_length()}, 
        object::pair{std:("updated"), std:("updated")}
    };
}

std::shared_ptr<Promise<object>> ReconciliationService::verifyQuoteState(string quoteId)
{
    auto result = std::async([=]() { this->reconcileQuote(quoteId); });
    return object{
        object::pair{std:("syncNeeded"), result["updated"]}
    };
}

std::shared_ptr<Promise<object>> ReconciliationService::healthCheck()
{
    if (!this->otcAddress) throw any(std::make_shared<Error>(std:("OTC address not configured")));
    auto blockNumber = std::async([=]() { this->client->getBlockNumber(); });
    std::async([=]() { this->client->readContract(object{
        object::pair{std:("address"), this->otcAddress}, 
        object::pair{std:("abi"), this->abi}, 
        object::pair{std:("functionName"), std:("nextOfferId")}, 
        object::pair{std:("args"), array<any>()}
    }); });
    return object{
        object::pair{std:("blockNumber"), Number(blockNumber)}, 
        object::pair{std:("contractAddress"), this->otcAddress}
    };
}

std::shared_ptr<ReconciliationService> getReconciliationService()
{
    if (!reconciliationServiceInstance) {
        reconciliationServiceInstance = std::make_shared<ReconciliationService>();
    }
    return reconciliationServiceInstance;
};


std::shared_ptr<Promise<void>> runReconciliationTask()
{
    console->log(std:("\
🔄 [Reconciliation Task] Starting...\
"));
    auto service = getReconciliationService();
    auto health = std::async([=]() { service->healthCheck(); });
    console->log(std:("[Reconciliation] Block: ") + health["blockNumber"] + std:(", Contract: ") + health["contractAddress"] + std:("\
"));
    auto result = std::async([=]() { service->reconcileAllActive(); });
    console->log(std:("\
✅ [Reconciliation] Complete: ") + result["updated"] + std:("/") + result["total"] + std:(" updated\
"));
    return std::shared_ptr<Promise<void>>();
};


any reconciliationServiceInstance = nullptr;

void Main(void)
{
}

MAIN
