#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "..entityId.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// Quote Service - quote management for Eliza OTC Desk
// Single source of truth registered with runtime.getService("QuoteService")

class QuoteService {
public:
    QuoteService(IAgentRuntime runtime);
    std::string serviceType() const;
    std::future<void> initialize();
    std::future<void> stop();
    std::future<QuoteService> start(IAgentRuntime runtime);
    std::future<void> addToIndex(const std::string& quoteId, const std::string& entityId, std::optional<std::string> beneficiary);
    std::string generateQuoteId(const std::string& entityId);
    std::string generateQuoteSignature(const std::any& data);
    std::future<std::vector<QuoteMemory>> getActiveQuotes();
    std::future<QuoteMemory> getQuoteByBeneficiary(const std::string& beneficiary);
    std::future<std::vector<QuoteMemory>> getUserQuoteHistory(const std::string& entityId, double limit);
    std::future<QuoteMemory> getQuoteByQuoteId(const std::string& quoteId);
    std::variant<Promise<QuoteMemory, null>> getQuoteByOfferId(const std::string& offerId);
    std::future<QuoteMemory> updateQuoteStatus(const std::string& quoteId, QuoteStatus status, const std::any& data);
    std::future<QuoteMemory> updateQuoteExecution(const std::string& quoteId, std::optional<std::any> data);
    std::future<QuoteMemory> setQuoteBeneficiary(const std::string& quoteId, const std::string& beneficiary);
    bool verifyQuoteSignature(QuoteMemory quote);
    std::variant<Promise<QuoteMemory, undefined>> getQuoteByWallet(const std::string& walletAddress);
    std::future<void> expireUserQuotes(const std::string& walletAddress);
};

// Helper to get service from runtime
std::optional<QuoteService> getQuoteService(IAgentRuntime runtime);


} // namespace elizaos
