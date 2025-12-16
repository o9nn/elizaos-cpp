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

import type { OTCConsignment } from "@/services/database";

// Sensitive fields that should not be exposed to buyers
// These reveal the seller's negotiation bounds which would let buyers game the system
const SENSITIVE_CONSIGNMENT_FIELDS = [
  "minDiscountBps",
  "maxDiscountBps",
  "minLockupDays",
  "maxLockupDays",
  "fixedDiscountBps",
  "fixedLockupDays",
  "minDealAmount",
  "maxDealAmount",
  "allowedBuyers",
] as const;

using SanitizedConsignment = std::variant<Omit<
  OTCConsignment,
  (typeof SENSITIVE_CONSIGNMENT_FIELDS)[number]
> & {
  termsType: "negotiable", "fixed">;
};

/**
 * Sanitize consignment to hide negotiation terms from non-owners.
 * This prevents buyers from gaming the negotiation by querying the API
 * to discover the seller's min/max discount and lockup bounds.
 */
;

  // Remove sensitive fields
  for (const field of SENSITIVE_CONSIGNMENT_FIELDS) {
    delete sanitized[field];
  }

  // Add a flag indicating whether terms are negotiable or fixed (but not the actual terms)
  sanitized.termsType = consignment.isNegotiable ? "negotiable" : "fixed";

  return sanitized as SanitizedConsignment;
}

/**
 * Check if a caller is the owner of a consignment.
 * Handles both Solana (case-sensitive) and EVM (case-insensitive) addresses.
 */


} // namespace elizaos
