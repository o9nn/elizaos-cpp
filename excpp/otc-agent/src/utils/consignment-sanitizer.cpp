#include "consignment-sanitizer.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

SanitizedConsignment sanitizeConsignmentForBuyer(OTCConsignment consignment) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, unknown> sanitized = { ...consignment };

    // Remove sensitive fields
    for (const auto& field : SENSITIVE_CONSIGNMENT_FIELDS)
        delete sanitized[field];
    }

    // Add a flag indicating whether terms are negotiable or fixed (but not the actual terms)
    sanitized.termsType = consignment.isNegotiable ? "negotiable" : "fixed";

    return sanitized;

}

bool isConsignmentOwner(OTCConsignment consignment, const std::optional<std::string>& callerAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!callerAddress) return false;

    const auto normalizedCaller =;
    consignment.chain == "solana";
    ? callerAddress;
    : callerAddress.toLowerCase();
    const auto normalizedConsigner =;
    consignment.chain == "solana";
    ? consignment.consignerAddress;
    : consignment.consignerAddress.toLowerCase();

    return normalizedCaller == normalizedConsigner;

}

} // namespace elizaos
