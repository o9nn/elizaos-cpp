#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "database.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * State Reconciliation Service
 *
 * Ensures database state matches blockchain contract state.
 * Critical for maintaining data integrity across the system.
 */

// Minimal client interface to avoid viem's "excessively deep" type issues
struct SimplePublicClient {
    Address address;
    Abi abi;
    std::string functionName;
    std::vector<readonly unknown> args;
};

// OnChainOffer matches the struct returned by the OTC contract
struct OnChainOffer {
    Address beneficiary;
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
    Address payer;
    bigint amountPaid;
};

class ReconciliationService {
public:
    ReconciliationService();
    std::future<OnChainOffer> readContractOffer(const std::variant<std::string, double>& offerId);
    Promise< reconcileQuote(const std::string& quoteId);
    void if(auto !dbQuote.offerId);
    Promise< reconcileAllActive();
    Promise< verifyQuoteState(const std::string& quoteId);
    Promise< healthCheck();

private:
    SimplePublicClient client_;
    Address otcAddress_;
    Abi abi_;
};

// Lazy singleton instance - only created when needed (at runtime, not build time)

ReconciliationService getReconciliationService();

std::future<void> runReconciliationTask();

} // namespace elizaos
