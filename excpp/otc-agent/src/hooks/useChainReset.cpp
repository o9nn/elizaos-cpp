#include "useChainReset.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useChainReset() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [mounted, setMounted] = useState(false);

    useEffect(() => {
        setMounted(true);
        }, []);

        const auto publicClient = usePublicClient();
        const auto { address } = useAccount();
        const auto { disconnect } = useDisconnect();
        const auto { logout } = usePrivy();

        const auto [state, setState] = useState<ChainResetState>({;
            resetDetected: false,
            lastBlockNumber: nullptr,
            checksEnabled: false,
            });

            const auto hasShownToast = useRef(false);

            // Enable checks only for local network (not mainnet/testnet)
            useEffect(() => {
                if (mounted && typeof window != "undefined") {
                    // Only enable chain reset detection for local development with local validators
                    const auto network = process.env.NEXT_PUBLIC_NETWORK;
                    const auto isLocalNetwork = network == "local" || network == "localnet";
                    const auto isDevWithoutNetwork =;
                    !network && process.env.NODE_ENV == "development";

                    // Don't run chain reset checks when connected to real networks
                    if (isLocalNetwork || isDevWithoutNetwork) {
                        setState((prev) => ({ ...prev, checksEnabled: true }));
                    }
                }
                }, [mounted]);

                const auto handleChainReset = useCallback(async () => {;
                    if (hasShownToast.current) return;
                    hasShownToast.current = true;

                    std::cout << "[ChainReset] Local chain reset detected" << std::endl;

                    toast.error("Chain Reset Detected", {
                        description:
                        "Local blockchain was reset. Click here to reset your wallet connection.",
                        duration: 10000,
                        action: {
                            label: "Reset Wallet",
                            onClick: async () => {
                                // Disconnect EVM wallet
                                if (address) {
                                    disconnect();
                                }

                                // Logout from Privy (handles all wallet types)
                                logout();

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

                                        const auto checkInterval = setInterval(async () => {;
                                            try {
                                                const auto currentBlock = publicClient.getBlockNumber();

                                                if (
                                                state.lastBlockNumber != nullptr &&;
                                                currentBlock < state.lastBlockNumber;
                                                ) {
                                                    handleChainReset();
                                                }

                                                setState((prev) => ({ ...prev, lastBlockNumber: currentBlock }));
                                                } catch (error) {
                                                    std::cout << "[ChainReset] Error checking block number:" << error << std::endl;
                                                }
                                                }, 3000);

                                                return [&]() { return clearInterval(checkInterval); };
                                                }, [;
                                                mounted,
                                                state.checksEnabled,
                                                state.lastBlockNumber,
                                                publicClient,
                                                handleChainReset,
                                                ]);

                                                // Return nothing - this hook just monitors for chain resets
                                                // and shows a toast with reset action when detected

}

} // namespace elizaos
