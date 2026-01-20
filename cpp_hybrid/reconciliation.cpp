#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/services/reconciliation.h"

ReconciliationService::ReconciliationService() {
    auto chain = getChain();
    auto rpcUrl = getRpcUrl();
    this->client = as<std::shared_ptr<SimplePublicClient>>(as<any>(createPublicClient(object{
        object::pair{std::string("chain"), std::string("chain")}, 
        object::pair{std::string("transport"), http(rpcUrl)}
    })));
    try
    {
        this->otcAddress = getContractAddress();
        console->log(std::string("[ReconciliationService] Using contract address: ") + this->otcAddress + std::string(" for network: ") + (OR((process->env->NETWORK), (std::string("localnet")))) + string_empty);
    }
    catch (const any& error)
    {
        console->error(std::string("[ReconciliationService] Failed to get contract address:"), error);
        throw any(error);
    }
    this->abi = as<std::shared_ptr<Abi>>(otcArtifact->abi);
}

std::shared_ptr<Promise<object>> ReconciliationService::reconcileQuote(string quoteId)
{
    auto dbQuote = std::async([=]() { QuoteDB::getQuoteByQuoteId(quoteId); });
    if (!dbQuote->offerId) {
        return object{
            object::pair{std::string("updated"), false}, 
            object::pair{std::string("oldStatus"), dbQuote->status}, 
            object::pair{std::string("newStatus"), dbQuote->status}
        };
    }
    auto contractOffer = std::async([=]() { this->readContractOffer(dbQuote->offerId); });
    auto contractStatus = (contractOffer->fulfilled) ? any(std::string("executed")) : any((contractOffer->cancelled) ? any(std::string("rejected")) : any((OR((contractOffer->paid), (contractOffer->approved))) ? std::string("approved") : std::string("active")));
    if (dbQuote->status == contractStatus) {
        return object{
            object::pair{std::string("updated"), false}, 
            object::pair{std::string("oldStatus"), dbQuote->status}, 
            object::pair{std::string("newStatus"), contractStatus}
        };
    }
    console->log(std::string("[Reconciliation] ") + quoteId + std::string(": ") + dbQuote->status + std::string(" → ") + contractStatus + string_empty);
    std::async([=]() { QuoteDB::updateQuoteStatus(quoteId, contractStatus, object{
        object::pair{std::string("offerId"), OR((dbQuote->offerId), (string_empty))}, 
        object::pair{std::string("transactionHash"), string_empty}, 
        object::pair{std::string("blockNumber"), 0}, 
        object::pair{std::string("rejectionReason"), string_empty}, 
        object::pair{std::string("approvalNote"), string_empty}
    }); });
    return object{
        object::pair{std::string("updated"), true}, 
        object::pair{std::string("oldStatus"), dbQuote->status}, 
        object::pair{std::string("newStatus"), contractStatus}
    };
}

std::shared_ptr<Promise<object>> ReconciliationService::reconcileAllActive()
{
    console->log(std::string("[Reconciliation] Starting reconciliation..."));
    auto activeQuotes = std::async([=]() { QuoteDB::getActiveQuotes(); });
    console->log(std::string("[Reconciliation] Found ") + activeQuotes->get_length() + std::string(" active quotes"));
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
    console->log(std::string("[Reconciliation] Complete: ") + updated + std::string("/") + results->get_length() + std::string(" updated"));
    return object{
        object::pair{std::string("total"), results->get_length()}, 
        object::pair{std::string("updated"), std::string("updated")}
    };
}

std::shared_ptr<Promise<object>> ReconciliationService::verifyQuoteState(string quoteId)
{
    auto result = std::async([=]() { this->reconcileQuote(quoteId); });
    return object{
        object::pair{std::string("syncNeeded"), result["updated"]}
    };
}

std::shared_ptr<Promise<object>> ReconciliationService::healthCheck()
{
    if (!this->otcAddress) throw any(std::make_shared<Error>(std::string("OTC address not configured")));
    auto blockNumber = std::async([=]() { this->client->getBlockNumber(); });
    std::async([=]() { this->client->readContract(object{
        object::pair{std::string("address"), this->otcAddress}, 
        object::pair{std::string("abi"), this->abi}, 
        object::pair{std::string("functionName"), std::string("nextOfferId")}, 
        object::pair{std::string("args"), array<any>()}
    }); });
    return object{
        object::pair{std::string("blockNumber"), Number(blockNumber)}, 
        object::pair{std::string("contractAddress"), this->otcAddress}
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
    console->log(std::string("\
🔄 [Reconciliation Task] Starting...\
"));
    auto service = getReconciliationService();
    auto health = std::async([=]() { service->healthCheck(); });
    console->log(std::string("[Reconciliation] Block: ") + health["blockNumber"] + std::string(", Contract: ") + health["contractAddress"] + std::string("\
"));
    auto result = std::async([=]() { service->reconcileAllActive(); });
    console->log(std::string("\
✅ [Reconciliation] Complete: ") + result["updated"] + std::string("/") + result["total"] + std::string(" updated\
"));
    return std::shared_ptr<Promise<void>>();
};


any reconciliationServiceInstance = nullptr;

void Main(void)
{
}

MAIN
