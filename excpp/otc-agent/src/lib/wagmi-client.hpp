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

;
import type { Config } from "wagmi";
;
;
;

// Custom RPC URLs - use proxy routes to keep API keys server-side
const baseRpcUrl = process.env.NEXT_PUBLIC_BASE_RPC_URL;
const bscRpcUrl = process.env.NEXT_PUBLIC_BSC_RPC_URL;
const anvilRpcUrl = process.env.NEXT_PUBLIC_RPC_URL || "http://127.0.0.1:8545";

// Get absolute URL for proxy routes (needed for wagmi HTTP transport)
${path}`;
  }
  // Server-side fallback - use env or default
  const appUrl = process.env.NEXT_PUBLIC_APP_URL || "http://localhost:4444";
  return `${appUrl}${path}`;
}

// Determine available chains based on configuration


  // Add Base chains (always available)
  chains.push(base, baseSepolia);

  // Add BSC chains (always available)
  chains.push(bsc, bscTestnet);

  return chains;
}

const chains = getAvailableChains();

// Build transports dynamically based on available chains
;

  const network = process.env.NEXT_PUBLIC_NETWORK;
  const isLocalNetwork = network === "local" || network === "localnet";

  // Only add localhost transport when explicitly using local network
  if (isLocalNetwork) {
    transports[localhost.id] = http(anvilRpcUrl);
  }

  // Add Base transports
  if (baseRpcUrl) {
    transports[base.id] = http(baseRpcUrl);
    transports[baseSepolia.id] = http(baseRpcUrl);
  } else {
    // Use proxy route for Base mainnet (keeps Alchemy API key server-side)
    // Falls back to public RPC if proxy isn't available
    transports[base.id] = http(getProxyUrl("/api/rpc/base"));
    transports[baseSepolia.id] = http("https://sepolia.base.org");
  }

  // Add BSC transports
  if (bscRpcUrl) {
    transports[bsc.id] = http(bscRpcUrl);
    transports[bscTestnet.id] = http(bscRpcUrl);
  } else {
    // Use public RPCs
    transports[bsc.id] = http("https://bsc-dataseed1.binance.org");
    transports[bscTestnet.id] = http(
      "https://data-seed-prebsc-1-s1.binance.org:8545",
    );
  }

  return transports;
}

// Create connectors only on client side to avoid indexedDB SSR errors
// Note: WalletConnect is handled by Privy, so we only use injected connector here
// farcasterMiniApp connector is prioritized when in Farcaster context
), // Fallback for browser wallets
  ];
}

// Wagmi configuration for Privy integration
// Privy handles wallet connection, wagmi handles contract interactions
const config: Config = createConfig({
  chains: chains as never,
  connectors: getConnectors(),
  transports: getTransports() as never,
  ssr: true,
});

// Export chains for UI reference
{ chains };

} // namespace elizaos
