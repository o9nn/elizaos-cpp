import type { Address } from "viem";
import { getOtcAddress } from "@/config/contracts";

/**
 * Get the appropriate OTC contract address based on network configuration.
 * 
 * @deprecated Use `getOtcAddress()` from `@/config/contracts` directly.
 * This wrapper exists for backwards compatibility.
 */
export function getContractAddress(): Address {
  return getOtcAddress() as Address;
}
