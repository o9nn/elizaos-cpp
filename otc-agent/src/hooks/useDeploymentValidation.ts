"use client";

import { useEffect } from "react";
import { isEVMChain, isSolanaChain } from "@/config/chains";
import { getChainConfig, Chain } from "@/config/chains";

export function useDeploymentValidation() {
  useEffect(() => {
    if (process.env.NODE_ENV === "development") return; // Skip in dev mode (auto-deployed)

    const chains: Chain[] = ["base", "solana"];
    const missing: string[] = [];

    for (const chain of chains) {
      const config = getChainConfig(chain);
      if (isEVMChain(chain)) {
        if (!config.contracts.otc) missing.push(`${chain} (OTC Contract)`);
      }
      if (isSolanaChain(chain)) {
        if (!config.contracts.otc) missing.push(`${chain} (Desk Address)`);
      }
    }

    if (missing.length > 0) {
      console.error(
        `❌ CRITICAL: Missing deployment configuration for: ${missing.join(", ")}. ` +
          `Please ensure deployment JSONs are present in src/config/deployments/ or env vars are set.`,
      );
      // Optional: Display a toast or blocking modal in UI
    }
  }, []);
}
