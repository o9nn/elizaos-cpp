import type { OTCConsignment } from "@/services/database";

// Sensitive fields that should not be exposed to buyers
// These reveal the seller's negotiation bounds which would let buyers game the system
export const SENSITIVE_CONSIGNMENT_FIELDS = [
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

export type SanitizedConsignment = Omit<
  OTCConsignment,
  (typeof SENSITIVE_CONSIGNMENT_FIELDS)[number]
> & {
  termsType: "negotiable" | "fixed";
};

/**
 * Sanitize consignment to hide negotiation terms from non-owners.
 * This prevents buyers from gaming the negotiation by querying the API
 * to discover the seller's min/max discount and lockup bounds.
 */
export function sanitizeConsignmentForBuyer(
  consignment: OTCConsignment,
): SanitizedConsignment {
  const sanitized: Record<string, unknown> = { ...consignment };

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
export function isConsignmentOwner(
  consignment: OTCConsignment,
  callerAddress: string | null | undefined,
): boolean {
  if (!callerAddress) return false;

  const normalizedCaller =
    consignment.chain === "solana"
      ? callerAddress
      : callerAddress.toLowerCase();
  const normalizedConsigner =
    consignment.chain === "solana"
      ? consignment.consignerAddress
      : consignment.consignerAddress.toLowerCase();

  return normalizedCaller === normalizedConsigner;
}
