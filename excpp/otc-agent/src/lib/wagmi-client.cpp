#include "wagmi-client.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string getProxyUrl(const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof window != "undefined") {
        return window.location.origin + path;
    }
    // Server-side fallback - use env or default
    const auto appUrl = process.env.NEXT_PUBLIC_APP_URL || "http://localhost:4444";
    return appUrl + path;

}

void getAvailableChains() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto network = process.env.NEXT_PUBLIC_NETWORK;
    const auto isLocalNetwork = network == "local" || network == "localnet";
    const auto chains = [];

    // Only add localhost when explicitly using local network
    if (isLocalNetwork) {
        chains.push_back(localhost);
    }

    // Add Base chains (always available)
    chains.push_back(base, baseSepolia);

    // Add BSC chains (always available)
    chains.push_back(bsc, bscTestnet);

    return chains;

}

void getTransports() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<double, ReturnType<typeof http>> transports = {};

    const auto network = process.env.NEXT_PUBLIC_NETWORK;
    const auto isLocalNetwork = network == "local" || network == "localnet";

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
                transports[bscTestnet.id] = http(;
                "https://data-seed-prebsc-1-s1.binance.org:8545",
                );
            }

            return transports;

}

void getConnectors() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof window == "undefined") return [];
    return [;
    farcasterMiniApp(), // Prioritize Farcaster wallet when in Farcaster Mini App context;
    injected({ shimDisconnect: true }), // Fallback for browser wallets
    ];

}

} // namespace elizaos
