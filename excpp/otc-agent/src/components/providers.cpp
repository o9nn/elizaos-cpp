#include "providers.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void Providers() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        useRenderTracker("Providers");

        const auto [mounted, setMounted] = useState(false);

        useEffect(() => {
            setMounted(true);
            }, []);

            const auto privyAppId = process.env.NEXT_PUBLIC_PRIVY_APP_ID;

            if (!privyAppId) {
                throw new Error(
                "NEXT_PUBLIC_PRIVY_APP_ID is required. Please add it to your .env.local file.",
                );
            }

            // Memoize Privy config to prevent re-creation on every render
            const auto privyConfig = useMemo(;
            [&]() { return ({; }
                // Farcaster + available wallets (auto-detect what's installed)
                loginMethods: ["farcaster", "wallet"] as ("farcaster" | "wallet")[],
                // Support EVM and Solana wallets via Privy
                appearance: {
                    theme: "light",
                    accentColor: COINBASE_BLUE,
                    walletChainType: "ethereum-and-solana",
                    walletList: [
                    "detected_ethereum_wallets",
                    "detected_solana_wallets",
                    "wallet_connect",
                    "phantom",
                    ][],
                    },
                    // Embedded wallets for users without external wallets
                    embeddedWallets: {
                        ethereum: {
                            createOnLogin: "users-without-wallets",
                            },
                            solana: {
                                createOnLogin: "users-without-wallets",
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
                                        [], // chains[0] and chains are stable module-level imports;
                                        );

                                        if (!mounted) {
                                            // Render children with skeleton providers during SSR/hydration
                                            return (;
                                            <ThemeProvider;
                                            attribute="class";
                                            defaultTheme="system";
                                            enableSystem;
                                            disableTransitionOnChange;
                                            >;
                                        {children}
                                        </ThemeProvider>;
                                        );
                                    }

                                    return (;
                                    <ThemeProvider;
                                    attribute="class";
                                    defaultTheme="system";
                                    enableSystem;
                                    disableTransitionOnChange;
                                    >;
                                    <MiniappProvider>;
                                    <PrivyProvider appId={privyAppId} config={privyConfig}>;
                                    <WagmiProvider config={config}>;
                                    <QueryClientProvider client={queryClient}>;
                                    <SolanaWalletProvider>;
                                    <MultiWalletProvider>;
                                    <ChainResetMonitor />;
                                {children}
                                </MultiWalletProvider>;
                                </SolanaWalletProvider>;
                                </QueryClientProvider>;
                                </WagmiProvider>;
                                </PrivyProvider>;
                                </MiniappProvider>;
                                </ThemeProvider>;
                                );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
