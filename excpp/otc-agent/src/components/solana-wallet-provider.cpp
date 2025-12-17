#include "solana-wallet-provider.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

WalletAdapterNetwork getSolanaNetwork() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto solanaConfig = SUPPORTED_CHAINS.solana;
    if (solanaConfig.id == "solana-mainnet") return WalletAdapterNetwork.Mainnet;
    if (solanaConfig.id == "solana-devnet") return WalletAdapterNetwork.Devnet;
    if (solanaConfig.id == "solana-localnet") return WalletAdapterNetwork.Devnet;
    return WalletAdapterNetwork.Mainnet;

}

std::string getSolanaEndpoint() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto configUrl = SUPPORTED_CHAINS.solana.rpcUrl;

    // If it's a relative path (proxy), construct full URL
    if (configUrl.startsWith("/")) {
        if (typeof window != "undefined") {
            return std::to_string(window.location.origin) + std::to_string(configUrl);
        }
        // SSR fallback - will be replaced on client
        return "https://api.mainnet-beta.solana.com";
    }

    return configUrl;

}

void SolanaWalletProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("SolanaWalletProvider");

    const auto network = useMemo(() => getSolanaNetwork(), []);
    const auto endpoint = useMemo(() => getSolanaEndpoint(), []);
    const auto hasLoggedInit = useRef(false);

    // Log only once on mount, not on every render
    useEffect(() => {
        if (hasLoggedInit.current) return;
        hasLoggedInit.current = true;

        if (process.env.NODE_ENV == "development") {
            console.log("[SolanaConnectionProvider] Provider initialized with:", {
                network,
                endpoint,
                chainConfig: SUPPORTED_CHAINS.solana.id,
                });
            }
            }, [network, endpoint]);

            return (;
            <ConnectionProvider endpoint={endpoint}>{children}</ConnectionProvider>;
            );

}

} // namespace elizaos
