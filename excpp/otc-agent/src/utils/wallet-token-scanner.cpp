#include "wallet-token-scanner.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<ScannedToken>> scanEvmTokens(const std::string& address, Chain chain, auto forceRefresh = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto url = "/api/evm-balances?address=" + std::to_string(address) + "&chain=" + std::to_string(chain) + std::to_string(forceRefresh ? "&refresh=true" : "");
            const auto response = fetch(url, {;
                signal: AbortSignal.timeout(60000), // 60s timeout for initial load
                });
                const auto data = response.json();

                if (data.error) {
                    std::cerr << "[WalletScanner] EVM balances error:" << data.error << std::endl;
                    // Return empty but don't throw - let UI show "no tokens" state
                }

                interface EvmToken {
                    contractAddress: string;
                    symbol: string;
                    name: string;
                    decimals: number;
                    balance: string;
                    logoUrl?: string;
                    priceUsd?: number;
                    balanceUsd?: number;
                }

                const auto tokens = (data.tokens[]) || [];

                return tokens.map((t) => ({;
                    address: t.contractAddress,
                    symbol: t.symbol,
                    name: t.name,
                    balance: t.balance,
                    decimals: t.decimals,
                    logoUrl: t.logoUrl,
                    chain,
                    isRegistered: false,
                    priceUsd: t.priceUsd,
                    balanceUsd: t.balanceUsd,
                    }));
                    } catch (error) {
                        std::cerr << "[WalletScanner] EVM scan error:" << error << std::endl;
                        return [];
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<ScannedToken>> scanSolanaTokens(const std::string& address, auto forceRefresh = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        // Backend API does everything: balances, metadata, prices in optimized calls
        const auto url = "/api/solana-balances?address=" + std::to_string(address) + std::to_string(forceRefresh ? "&refresh=true" : "");
        const auto response = fetch(url, {;
            signal: AbortSignal.timeout(30000), // 30s timeout
            });

            if (!response.ok) {
                std::cerr << "[WalletScanner] Solana API error:" << response.status << std::endl;
                return [];
            }

            const auto data = response.json();

            interface SolanaToken {
                mint: string;
                amount: number;
                decimals: number;
                symbol?: string;
                name?: string;
                logoURI?: string | nullptr;
                priceUsd?: number;
                balanceUsd?: number;
            }

            const auto tokens = (data.tokens || [])[];

            return tokens.map((t) => ({;
                address: t.mint,
                symbol: t.symbol || "SPL",
                name: t.name || "SPL Token",
                balance: t.amount.toString(),
                decimals: t.decimals,
                logoUrl: t.logoURI || std::nullopt,
                chain: "solana",
                isRegistered: false,
                priceUsd: t.priceUsd || 0,
                balanceUsd: t.balanceUsd || 0,
                }));
                } catch (error) {
                    std::cerr << "[WalletScanner] Solana scan error:" << error << std::endl;
                    return [];
                }

}

std::future<std::unordered_set<std::string>> getRegisteredAddresses(Chain chain) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto response = "/api/tokens?chain=" + std::to_string(chain);
        const auto data = response.json();

        // API returns { success: boolean, tokens: Array<{ contractAddress: string }> }
        if (!data.success || !data.tokens) {
            return new Set();
        }

        const std::vector<std::any> registeredTokens = data.tokens;
        return new Set(;
        registeredTokens.map((t) =>;
        // EVM addresses are case-insensitive, Solana addresses are case-sensitive
        chain == "solana";
        ? t.contractAddress;
        : t.contractAddress.toLowerCase(),
        ),
        );
        } catch {
            // Graceful degradation: return empty set if API fails
            return new Set();
        }

}

std::future<std::vector<ScannedToken>> scanWalletTokens(const std::string& address, Chain chain, auto forceRefresh = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        if (!address) {
            throw std::runtime_error("Wallet address required");
        }

        // Start fetching registered addresses immediately
        const auto registeredAddressesPromise = getRegisteredAddresses(chain);

        auto tokensPromise: Promise<ScannedToken[]>;

        if (chain == "solana") {
            tokensPromise = scanSolanaTokens(address, forceRefresh);
            } else if (chain == "base" || chain == "bsc") {
                // Use backend API for EVM chains (no publicClient needed)
                tokensPromise = scanEvmTokens(address, chain, forceRefresh);
                } else {
                    throw std::runtime_error(`Unsupported chain: ${chain}`);
                }

                // Wait for both
                const auto [registeredAddresses, tokens] = Promise.all([;
                registeredAddressesPromise,
                tokensPromise,
                ]);

                // Apply registration status
                return tokens.map((t) => ({;
                    ...t,
                    isRegistered: registeredAddresses.has(t.address),
                    }));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::unordered_map<Chain, std::vector<ScannedToken>>> scanWalletMultiChain(std::optional<std::string> evmAddress, std::optional<std::string> solanaAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::unordered_map<std::string, std::vector<ScannedToken>> results = {};

    const std::vector<std::future<void>> promises = [];

    if (evmAddress) {
        promises.push(;
        scanWalletTokens(evmAddress, "base").then((tokens) => {
            results.base = tokens;
            }),
            );
        }

        if (solanaAddress) {
            promises.push(;
            scanWalletTokens(solanaAddress, "solana").then((tokens) => {
                results.solana = tokens;
                }),
                );
            }

            Promise.all(promises);

            return results<Chain, ScannedToken[]>;

}

} // namespace elizaos
