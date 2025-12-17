#include "quote.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<QuoteMemory> getUserQuote(const std::string& walletAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto { agentRuntime } = import("../../agent-runtime");
    const auto runtime = agentRuntime.getRuntime();

    // Use QuoteService to get quote (it has the correct ID generation logic)
    const auto quoteService = runtime.getService<any>("QuoteService");
    if (!quoteService) {
        return std::nullopt;
    }

    const auto quote = quoteService.getQuoteByWallet(walletAddress);
    return quote;

}

std::future<void> deleteUserQuote(const std::string& walletAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalized = walletAddress.toLowerCase();
    const auto entityId = walletToEntityId(normalized);

    std::cout << "[deleteUserQuote] Deleting quote for wallet:" << normalized << std::endl;

    const auto runtime = agentRuntime.getRuntime();
    const auto quote = getUserQuote(walletAddress);

    if (!quote) {
        std::cout << "[deleteUserQuote] No quote found to delete" << std::endl;
        return;
    }

    "runtime.deleteCache(" + "quote:" + quote.quoteId

    const auto entityQuoteIds =;
    "(runtime.getCache<string[]>(" + "entity_quotes:" + entityId
    const auto updatedEntityQuoteIds = entityQuoteIds.filter(;
    [&](id) { return id != quote.quoteId,; }
    );
    "runtime.setCache(" + "entity_quotes:" + entityId

    // Also remove from beneficiary index
    const auto beneficiaryQuoteIds =;
    "(runtime.getCache<string[]>(" + "beneficiary_quotes:" + normalized
    [];
    const auto updatedBeneficiaryQuoteIds = beneficiaryQuoteIds.filter(;
    [&](id) { return id != quote.quoteId,; }
    );
    runtime.setCache(;
    "beneficiary_quotes:" + normalized
    updatedBeneficiaryQuoteIds,
    );

    const auto allQuotes = (runtime.getCache<string[]>("all_quotes")) || [];
    const auto updatedAllQuotes = allQuotes.filter((id) => id != quote.quoteId);
    runtime.setCache("all_quotes", updatedAllQuotes);

    std::cout << "[deleteUserQuote] ✅ Quote deleted:" << quote.quoteId << std::endl;

}

} // namespace elizaos
