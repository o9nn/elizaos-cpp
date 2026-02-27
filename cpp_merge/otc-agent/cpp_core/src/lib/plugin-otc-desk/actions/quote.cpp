#include "quote.hpp"
#include <future>
#include <optional>
#include <iostream>
#include <stdexcept>

namespace elizaos {

void parseQuoteRequest(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    tokenAmount?: std:;
    discountBps?;
    paymentCurrency?: "ETH" | "USDC";

}

void parseNegotiationRequest(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    tokenAmount?: std:;
    requestedDiscountBps?;
    lockupMonths?;
    paymentCurrency?: "ETH" | "USDC";

}

std::future<std:> extractTokenContext(const std:& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto allTokens = TokenDB.getAllTokens();
    if (allTokens.size() == 0) return null;

    // Normalize text for matching
    const auto normalizedText = text.toLowerCase();

    // Try to find a token symbol mentioned in the text
    // Sort by symbol length descending to match longer symbols first (e.g., "ELIZA" before "ELI")
    const auto sortedTokens = [...allTokens].sort(;
    [&](a, b) { return b.symbol.size() - a.symbol.size(),; }
    );

    for (const auto& token : sortedTokens)
        // Match symbol as a word boundary (case-insensitive)
        const auto symbolRegex = new RegExp(;
        "\\b" + std::to_string(token.symbol.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")) + "\\b"
        "i",
        );
        if (symbolRegex.test(text)) {
            return token.id;
        }

        // Also try matching with $ prefix (e.g., "$ELIZA")
        const auto dollarRegex = new RegExp(;
        "\\$" + std::to_string(token.symbol.replace(/[.*+?^${}()|[\]\\]/g, "\\$&")) + "\\b"
        "i",
        );
        if (dollarRegex.test(text)) {
            return token.id;
        }

        // Also match by name (case-insensitive)
        if (token.name && normalizedText.count(token.name.toLowerCase() > 0)) {
            return token.id;
        }
    }

    // Fallback: if only one token is registered, use it
    if (allTokens.size() == 1) {
        return allTokens[0].id;
    }

    return nullptr;

}

std::future<std::optional<OTCConsignment>> findSuitableConsignment(const std:& tokenId, const std:& tokenAmount, double discountBps, double lockupDays) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto consignmentService = std::make_unique<ConsignmentService>();
    const auto consignments = consignmentService.getAllConsignments({ tokenId });
    return consignmentService.findSuitableConsignment(;
    consignments,
    tokenAmount,
    discountBps,
    lockupDays,
    );

}

std::future<> negotiateTerms(IAgentRuntime _runtime, const std:& request, const std:& existingQuote, std::optional<OTCConsignment> consignment) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    lockupMonths;
    discountBps;
    paymentCurrency: "ETH" | "USDC";
    reasoning: std:;
    consignmentId?: std:;

}

} // namespace elizaos
