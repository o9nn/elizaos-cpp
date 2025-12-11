"use client";

import { useEffect, useRef, useState, useCallback } from "react";
import { usePublicClient, useAccount, useDisconnect } from "wagmi";
import { usePrivy } from "@privy-io/react-auth";
import { toast } from "sonner";

type ChainResetState = {
  resetDetected: boolean;
  lastBlockNumber: bigint | null;
  checksEnabled: boolean;
};

export function useChainReset() {
  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
  }, []);

  const publicClient = usePublicClient();
  const { address } = useAccount();
  const { disconnect } = useDisconnect();
  const { logout } = usePrivy();

  const [state, setState] = useState<ChainResetState>({
    resetDetected: false,
    lastBlockNumber: null,
    checksEnabled: false,
  });

  const hasShownToast = useRef(false);

  // Enable checks only for local network (not mainnet/testnet)
  useEffect(() => {
    if (mounted && typeof window !== "undefined") {
      // Only enable chain reset detection for local development with local validators
      const network = process.env.NEXT_PUBLIC_NETWORK;
      const isLocalNetwork = network === "local" || network === "localnet";
      const isDevWithoutNetwork =
        !network && process.env.NODE_ENV === "development";

      // Don't run chain reset checks when connected to real networks
      if (isLocalNetwork || isDevWithoutNetwork) {
        setState((prev) => ({ ...prev, checksEnabled: true }));
      }
    }
  }, [mounted]);

  const handleChainReset = useCallback(async () => {
    if (hasShownToast.current) return;
    hasShownToast.current = true;

    console.warn("[ChainReset] Local chain reset detected");

    toast.error("Chain Reset Detected", {
      description:
        "Local blockchain was reset. Click here to reset your wallet connection.",
      duration: 10000,
      action: {
        label: "Reset Wallet",
        onClick: async () => {
          // Disconnect EVM wallet
          if (address) {
            await disconnect();
          }

          // Logout from Privy (handles all wallet types)
          await logout();

          // Clear all wallet caches
          localStorage.removeItem("wagmi.store");
          localStorage.removeItem("wagmi.cache");
          localStorage.removeItem("wagmi.recentConnectorId");
          localStorage.removeItem("privy:token");
          localStorage.removeItem("privy:refresh_token");

          setTimeout(() => {
            window.location.reload();
          }, 500);
        },
      },
    });

    setState((prev) => ({ ...prev, resetDetected: true }));
  }, [address, disconnect, logout]);

  useEffect(() => {
    if (!mounted || !state.checksEnabled || !publicClient) return;

    const checkInterval = setInterval(async () => {
      try {
        const currentBlock = await publicClient.getBlockNumber();

        if (
          state.lastBlockNumber !== null &&
          currentBlock < state.lastBlockNumber
        ) {
          await handleChainReset();
        }

        setState((prev) => ({ ...prev, lastBlockNumber: currentBlock }));
      } catch (error) {
        console.warn("[ChainReset] Error checking block number:", error);
      }
    }, 3000);

    return () => clearInterval(checkInterval);
  }, [
    mounted,
    state.checksEnabled,
    state.lastBlockNumber,
    publicClient,
    handleChainReset,
  ]);

  // Return nothing - this hook just monitors for chain resets
  // and shows a toast with reset action when detected
}
