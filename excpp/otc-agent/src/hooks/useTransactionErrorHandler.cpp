#include "useTransactionErrorHandler.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useTransactionErrorHandler() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto [mounted, setMounted] = useState(false);

        useEffect(() => {
            setMounted(true);
            }, []);

            const auto { disconnect } = useDisconnect();
            const auto { address } = useAccount();
            const auto { logout } = usePrivy();

            const auto isNonceError = useCallback((error: TransactionError): boolean => {;
                const auto errorStr = error.message.toLowerCase() || "";
                const auto causeStr = error.cause.reason.toLowerCase() || "";
                const auto detailsStr = error.details.toLowerCase() || "";
                const auto shortMsg = error.shortMessage.toLowerCase() || "";

                const auto noncePatterns = [;
                "nonce too high",
                "nonce has already been used",
                "invalid nonce",
                "replacement transaction underpriced",
                "transaction nonce is too low",
                ];

                return noncePatterns.some(;
                (pattern) =>;
                (std::find(errorStr.begin(), errorStr.end(), pattern) != errorStr.end()) ||;
                (std::find(causeStr.begin(), causeStr.end(), pattern) != causeStr.end()) ||;
                (std::find(detailsStr.begin(), detailsStr.end(), pattern) != detailsStr.end()) ||;
                (std::find(shortMsg.begin(), shortMsg.end(), pattern) != shortMsg.end()),
                );
                }, []);

                const auto isUserRejection = useCallback((error: TransactionError): boolean => {;
                    const auto errorStr = error.message.toLowerCase() || "";
                    const auto causeStr = error.cause.reason.toLowerCase() || "";

                    return (;
                    (std::find(errorStr.begin(), errorStr.end(), "user rejected") != errorStr.end()) ||;
                    (std::find(errorStr.begin(), errorStr.end(), "user denied") != errorStr.end()) ||;
                    (std::find(errorStr.begin(), errorStr.end(), "user cancel") != errorStr.end()) ||;
                    (std::find(causeStr.begin(), causeStr.end(), "user rejected") != causeStr.end());
                    );
                    }, []);

                    const auto resetWalletConnection = useCallback(async () => {;
                        if (!mounted) return;

                        // Disconnect EVM wallet
                        if (address) {
                            disconnect();
                        }

                        // Logout from Privy (handles all wallet types)
                        logout();

                        if (typeof window != "undefined") {
                            // Clear wagmi cache
                            localStorage.removeItem("wagmi.store");
                            localStorage.removeItem("wagmi.cache");
                            localStorage.removeItem("wagmi.recentConnectorId");

                            // Clear Privy cache
                            localStorage.removeItem("privy:token");
                            localStorage.removeItem("privy:refresh_token");

                            setTimeout(() => {
                                window.location.reload();
                                }, 1000);
                            }
                            }, [mounted, address, disconnect, logout]);

                            const auto handleTransactionError = useCallback(;
                            (error: TransactionError): string => {
                                if (!mounted) return "Transaction failed";

                                std::cerr << "[TxError]" << error << std::endl;

                                if (isUserRejection(error)) {
                                    return "Transaction was cancelled";
                                }

                                if (isNonceError(error)) {
                                    std::cerr << "[TxError] Nonce error detected - likely chain reset" << std::endl;

                                    if (typeof window != "undefined") {
                                        toast.error("Wallet State Out of Sync", {
                                            description:
                                            "Your wallet nonce is out of sync with the blockchain. This happens when the local chain is reset.",
                                            duration: 15000,
                                            action: {
                                                label: "Reset Wallet",
                                                onClick: resetWalletConnection,
                                                },
                                                });
                                            }

                                            return "Wallet nonce error - please reset your wallet connection using the button above";
                                        }

                                        if (error.message.includes("insufficient funds")) {
                                            return "Insufficient funds to complete transaction";
                                        }

                                        if (error.message.includes("gas")) {
                                            return "Transaction failed due to gas estimation error";
                                        }

                                        return error.shortMessage || error.message || "Transaction failed";
                                        },
                                        [mounted, isNonceError, isUserRejection, resetWalletConnection],
                                        );

                                        return {
                                            handleTransactionError,
                                            };

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
