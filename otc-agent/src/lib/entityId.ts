// Entity ID utilities using Eliza's stringToUuid

import { stringToUuid } from "@elizaos/core";

/**
 * Detect if an address is a Solana address (Base58 encoded)
 * Solana addresses are case-sensitive and don't start with 0x
 */
function isSolanaAddress(address: string): boolean {
  // Solana addresses are Base58 (no 0x prefix, typically 32-44 chars)
  // They contain alphanumeric chars but no 0, O, I, l (Base58)
  return (
    !address.startsWith("0x") && address.length >= 32 && address.length <= 44
  );
}

/**
 * Convert wallet address to deterministic UUID entity ID
 * Uses Eliza's built-in stringToUuid for consistency with runtime
 *
 * IMPORTANT: EVM addresses are case-insensitive (lowercased for consistency)
 * Solana addresses are case-sensitive (Base58 encoded, preserved as-is)
 */
export function walletToEntityId(address: string): string {
  const trimmed = address.trim();
  // Preserve case for Solana addresses, lowercase for EVM
  const normalized = isSolanaAddress(trimmed) ? trimmed : trimmed.toLowerCase();
  return stringToUuid(normalized) as string;
}
