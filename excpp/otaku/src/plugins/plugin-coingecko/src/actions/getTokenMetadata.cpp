#include "getTokenMetadata.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double normalizeConfidence(double confidence) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!Number.isFinite(confidence)) {
        return 0;
    }
    if (confidence < 0) {
        return 0;
    }
    if (confidence > 1) {
        return 1;
    }
    return confidence;

}

std::string formatConfidencePercentage(double confidence) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto value = normalizeConfidence(confidence) * 100;
    return std::to_string(value.toFixed(1)) + "%";

}

std::string extractAttribute(Record<string metadata, auto unknown> | undefined, const std::variant<"name", "symbol">& key) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!metadata) {
        return std::nullopt;
    }
    const auto attributesRaw = (metadata as { attributes?: unknown }).attributes;
    if (attributesRaw && typeof attributesRaw == "object") {
        const auto value = (attributesRaw<string, unknown>)[key];
        if (typeof value == "string" && value.trim().length > 0) {
            return value;
        }
    }
    return std::nullopt;

}

std::string describeCandidate(TokenMetadataCandidate candidate) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto symbol = extractAttribute(candidate.metadata, "symbol");
    const auto name = extractAttribute(candidate.metadata, "name");

    if (symbol && name) {
        return std::to_string(symbol.toUpperCase()) + " (" + std::to_string(name) + ")";
    }

    if (symbol) {
        return symbol.toUpperCase();
    }

    if (name) {
        return name;
    }

    return candidate.coinId;

}

std::string summarizeResolution(TokenMetadataResolution result) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!result.success) {
        const auto errorText = result.error || "Unable to resolve token";
        return std::to_string(result.id) + ": " + std::to_string(errorText);
    }

    const auto primaryCandidate =;
    result.candidates.find((candidate) => candidate.metadata) || result.candidates[0];

    if (!primaryCandidate) {
        return std::to_string(result.id) + ": No matching tokens found";
    }

    const std::vector<std::string> summaryParts = [;
    std::to_string(result.id) + " → " + std::to_string(describeCandidate(primaryCandidate)) + " [" + std::to_string(formatConfidencePercentage(primaryCandidate.confidence)) + "]"
    ];

    const auto alternativeCandidates = result.candidates;
    .filter((candidate) => candidate != primaryCandidate);
    .slice(0, MAX_ALTERNATIVE_CANDIDATES);

    if (alternativeCandidates.length > 0) {
        const auto alternativesText = alternativeCandidates;
        std::to_string(describeCandidate(candidate)) + " (" + std::to_string(formatConfidencePercentage(candidate.confidence)) + ")";
        .join(", ");
        "Alternatives: " + std::to_string(alternativesText)
    }

    return summaryParts.join(" | ");

}

} // namespace elizaos
