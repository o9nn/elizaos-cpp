#include "quote.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void parseQuoteRequest(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    tokenAmount?: string;
    discountBps?: number;
    paymentCurrency?: "ETH" | "USDC";

}

void parseNegotiationRequest(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    tokenAmount?: string;
    requestedDiscountBps?: number;
    lockupMonths?: number;
    paymentCurrency?: "ETH" | "USDC";

}

std::future<std::string> extractTokenContext(const std::string& text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto allTokens = TokenDB.getAllTokens();
    if (allTokens.length == 0) return null;

    // Normalize text for matching
    const auto normalizedText = text.toLowerCase();

    // Try to find a token symbol mentioned in the text
    // Sort by symbol length descending to match longer symbols first (e.g., "ELIZA" before "ELI")
    const auto sortedTokens = [...allTokens].sort(;
    [&](a, b) { return b.symbol.length - a.symbol.length,; }
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
        if (token.name && normalizedText.includes(token.name.toLowerCase())) {
            return token.id;
        }
    }

    // Fallback: if only one token is registered, use it
    if (allTokens.length == 1) {
        return allTokens[0].id;
    }

    return nullptr;

}

std::future<std::optional<OTCConsignment>> findSuitableConsignment(const std::string& tokenId, const std::string& tokenAmount, double discountBps, double lockupDays) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto consignmentService = new ConsignmentService();
    const auto consignments = consignmentService.getAllConsignments({ tokenId });
    return consignmentService.findSuitableConsignment(;
    consignments,
    tokenAmount,
    discountBps,
    lockupDays,
    );

}

std::future<> negotiateTerms(IAgentRuntime _runtime, const std::any& request, const std::any& existingQuote, std::optional<OTCConsignment> consignment) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    lockupMonths: number;
    discountBps: number;
    paymentCurrency: "ETH" | "USDC";
    reasoning: string;
    consignmentId?: string;

}

} // namespace elizaos
