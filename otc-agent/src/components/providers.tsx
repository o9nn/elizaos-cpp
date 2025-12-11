"use client";

import { MultiWalletProvider } from "@/components/multiwallet";
import { ChainResetMonitor } from "@/components/chain-reset-monitor";
import { SolanaWalletProvider } from "@/components/solana-wallet-provider";
import { MiniappProvider } from "@/components/miniapp-provider";
import { config, chains } from "@/lib/wagmi-client";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { ThemeProvider } from "next-themes";
import { useEffect, useMemo, useState } from "react";
import { WagmiProvider } from "wagmi";
import { PrivyProvider, type WalletListEntry } from "@privy-io/react-auth";
import { toSolanaWalletConnectors } from "@privy-io/react-auth/solana";
import { useRenderTracker } from "@/utils/render-tracker";

// Theme colors for external library configs (Privy)
const COINBASE_BLUE = "#0052ff" as `#${string}`;

// Create query client once, outside component to prevent re-creation on renders
const queryClient = new QueryClient({
  defaultOptions: {
    queries: {
      staleTime: 10000, // Consider data fresh for 10s
      gcTime: 60000, // Keep unused data for 60s before garbage collection
      refetchOnWindowFocus: true, // Refetch when tab regains focus
    },
  },
});

// Memoize Solana connectors at module level to prevent recreation
const solanaConnectors = toSolanaWalletConnectors();

export function Providers({ children }: { children: React.ReactNode }) {
  useRenderTracker("Providers");

  const [mounted, setMounted] = useState(false);

  useEffect(() => {
    setMounted(true);
  }, []);

  const privyAppId = process.env.NEXT_PUBLIC_PRIVY_APP_ID;

  if (!privyAppId) {
    throw new Error(
      "NEXT_PUBLIC_PRIVY_APP_ID is required. Please add it to your .env.local file.",
    );
  }

  // Memoize Privy config to prevent re-creation on every render
  const privyConfig = useMemo(
    () => ({
      // Farcaster + available wallets (auto-detect what's installed)
      loginMethods: ["farcaster", "wallet"] as ("farcaster" | "wallet")[],
      // Support EVM and Solana wallets via Privy
      appearance: {
        theme: "light" as const,
        accentColor: COINBASE_BLUE,
        walletChainType: "ethereum-and-solana" as const,
        walletList: [
          "detected_ethereum_wallets",
          "detected_solana_wallets",
          "wallet_connect",
          "phantom",
        ] as WalletListEntry[],
      },
      // Embedded wallets for users without external wallets
      embeddedWallets: {
        ethereum: {
          createOnLogin: "users-without-wallets" as const,
        },
        solana: {
          createOnLogin: "users-without-wallets" as const,
        },
      },
      defaultChain: chains[0],
      supportedChains: chains,
      externalWallets: {
        solana: {
          connectors: solanaConnectors,
        },
      },
    }),
    [], // chains[0] and chains are stable module-level imports
  );

  if (!mounted) {
    // Render children with skeleton providers during SSR/hydration
    return (
      <ThemeProvider
        attribute="class"
        defaultTheme="system"
        enableSystem
        disableTransitionOnChange
      >
        {children}
      </ThemeProvider>
    );
  }

  return (
    <ThemeProvider
      attribute="class"
      defaultTheme="system"
      enableSystem
      disableTransitionOnChange
    >
      <MiniappProvider>
        <PrivyProvider appId={privyAppId} config={privyConfig}>
          <WagmiProvider config={config}>
            <QueryClientProvider client={queryClient}>
              <SolanaWalletProvider>
                <MultiWalletProvider>
                  <ChainResetMonitor />
                  {children}
                </MultiWalletProvider>
              </SolanaWalletProvider>
            </QueryClientProvider>
          </WagmiProvider>
        </PrivyProvider>
      </MiniappProvider>
    </ThemeProvider>
  );
}
