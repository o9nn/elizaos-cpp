#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Sensitive fields that should not be exposed to buyers
// These reveal the seller's negotiation bounds which would let buyers game the system

using SanitizedConsignment = std::variant<Omit<

/**
 * Sanitize consignment to hide negotiation terms from non-owners.
 * This prevents buyers from gaming the negotiation by querying the API
 * to discover the seller's min/max discount and lockup bounds.
 */
SanitizedConsignment sanitizeConsignmentForBuyer(OTCConsignment consignment);

/**
 * Check if a caller is the owner of a consignment.
 * Handles both Solana (case-sensitive) and EVM (case-insensitive) addresses.
 */
bool isConsignmentOwner(OTCConsignment consignment, string | null | undefined callerAddress);

} // namespace elizaos
