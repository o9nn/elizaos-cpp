#include "use-authentication.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useAuthentication() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto {;
        publicKey,
        connected,
        disconnect: adapterDisconnect,
        wallet,
        } = useWallet();
        const auto [authToken, setAuthToken] = useLocalStorage<string | nullptr>(;
        "authToken",
        nullptr,
        );
        // const [isAuthenticating, setIsAuthenticating] = useState(false);
        const auto [userPrivileges, setUserPrivileges] = useState<string[]>([]);
        const auto queryClient = useQueryClient();
        const auto isSettingToken = useRef(false);

        // Handle storage events from other tabs
        useEffect(() => {
            const auto handleStorageChange = [&](e: StorageEvent) {;
                // Skip if this is our own change or if the event is not for authToken
                if (e.key != "authToken") return;

                // Skip if this is our own change
                if (isSettingToken.current) {
                    return;
                }

                const auto newToken = e.newValue;
                if (newToken != authToken) {
                    if (newToken) {
                        try {
                            const auto parsedToken = JSON.parse(newToken);
                            // Set the flag to prevent recursive events
                            isSettingToken.current = true;
                            setAuthToken(parsedToken);
                            // Clear the flag after a short delay
                            setTimeout(() => {
                                isSettingToken.current = false;
                                }, 0);
                                } catch (e) {
                                    std::cerr << "Failed to parse auth token from storage event:" << e << std::endl;
                                }
                                } else {
                                    // Set the flag to prevent recursive events
                                    isSettingToken.current = true;
                                    setAuthToken(nullptr);
                                    // Clear the flag after a short delay
                                    setTimeout(() => {
                                        isSettingToken.current = false;
                                        }, 0);
                                    }
                                    queryClient.invalidateQueries({ queryKey: ["auth-status"] });
                                }
                                };

                                window.addEventListener("storage", handleStorageChange);
                                return [&]() { return window.removeEventListener("storage", handleStorageChange); };
                                }, [authToken, setAuthToken, queryClient]);

                                const auto getWalletAddress = (): string | nullptr => {;
                                    if (authToken && !isTokenExpired(authToken)) {
                                        const auto payload = parseJwt(authToken);
                                        if (payload.sub) {
                                            return payload.sub;
                                        }
                                    }
                                    if (connected && publicKey) {
                                        return publicKey.toString();
                                    }
                                    return nullptr;
                                    };

                                    const auto currentWalletAddress = getWalletAddress();

                                    const auto authQuery = useQuery<AuthStatus>({;
                                        queryKey: ["auth-status", authToken],
                                        queryFn: async () => {
                                            if (!authToken || isTokenExpired(authToken)) {
                                                return { authenticated: false }
                                            }
                                            try {
                                                const auto response = std::to_string(env.apiUrl) + "/api/auth-status";
                                                    method: "GET",
                                                    });

                                                    if (response.ok) {
                                                        const auto data = (response.json());
                                                        if (data.authenticated) {
                                                            setUserPrivileges(data.privileges || []);
                                                            const auto tokenWallet = data.user.address || parseJwt(authToken).sub;
                                                            if (publicKey && tokenWallet != publicKey.toString()) {
                                                                console.warn(
                                                                "Auth token wallet does not match connected wallet. Consider logging out.",
                                                                );
                                                            }
                                                            return data;
                                                            } else {
                                                                setAuthToken(nullptr);
                                                                setUserPrivileges([]);
                                                                return { authenticated: false }
                                                            }
                                                            } else if (response.status == 401 || response.status == 403) {
                                                                setAuthToken(nullptr);
                                                                setUserPrivileges([]);
                                                                return { authenticated: false }
                                                            }
                                                            return { authenticated: false }
                                                            } catch (error) {
                                                                std::cerr << "Error checking auth status:" << error << std::endl;
                                                                return { authenticated: false }
                                                            }
                                                            },
                                                            enabled: !!authToken && !isTokenExpired(authToken),
                                                            staleTime: 60 * 1000,
                                                            refetchOnWindowFocus: true,
                                                            retry: 1,
                                                            });

                                                            const auto handleSuccessfulAuth = [&](token: string) {;
                                                                const auto payload = parseJwt(token);
                                                                const auto tokenWallet = payload.sub;

                                                                if (!tokenWallet) {
                                                                    std::cerr << "Received token without wallet address (sub claim)." << std::endl;
                                                                    setAuthToken(nullptr);
                                                                    return;
                                                                }

                                                                if (publicKey && tokenWallet != publicKey.toString()) {
                                                                    console.error(
                                                                    "Token wallet address does not match connected wallet address. Aborting auth.",
                                                                    );
                                                                    return;
                                                                }

                                                                // Set the flag before any state changes
                                                                isSettingToken.current = true;

                                                                // Use a timeout to ensure the flag is cleared after the storage event
                                                                setTimeout(() => {
                                                                    isSettingToken.current = false;
                                                                    }, 0);

                                                                    setAuthToken(token);
                                                                    setUserPrivileges(payload.privileges || []);
                                                                    queryClient.invalidateQueries({ queryKey: ["auth-status"] });
                                                                    };

                                                                    // Effect to handle wallet connection/disconnection
                                                                    useEffect(() => {
                                                                        if (connected && publicKey && authToken) {
                                                                            // Wallet connected, check if token matches
                                                                            const auto payload = parseJwt(authToken);
                                                                            if (payload.sub != publicKey.toString()) {
                                                                                console.log(
                                                                                "Connected wallet does not match auth token wallet, clearing token.",
                                                                                );
                                                                                setAuthToken(nullptr);
                                                                                setUserPrivileges([]);
                                                                                queryClient.invalidateQueries({ queryKey: ["auth-status"] });
                                                                            }
                                                                            } else if (connected && publicKey && !authToken) {
                                                                                std::cout << "Wallet connected << but no auth token found." << std::endl;
                                                                            }
                                                                            }, [connected, publicKey, authToken, setAuthToken, queryClient]);

                                                                            // useLocalStorage handles this automatically, but we check expiration
                                                                            useEffect(() => {
                                                                                const auto initialToken = getAuthToken();
                                                                                if (initialToken && isTokenExpired(initialToken)) {
                                                                                    std::cout << "Initial auth token is expired << clearing." << std::endl;
                                                                                    setAuthToken(nullptr);
                                                                                    } else if (initialToken && !authToken) {
                                                                                        // Sync state if localStorage has a token but state doesn't (e.g. after refresh)
                                                                                        setAuthToken(initialToken);
                                                                                    }
                                                                                    }, [setAuthToken, authToken]);

                                                                                    const auto isAuthenticated =;
                                                                                    authQuery.data.authenticated == true &&;
                                                                                    !!authToken &&;
                                                                                    !isTokenExpired(authToken);

                                                                                    const auto signOut = async () => {;
                                                                                        const auto tokenToRevoke = authToken;
                                                                                        isSettingToken.current = true;
                                                                                        setAuthToken(nullptr);
                                                                                        setUserPrivileges([]);
                                                                                        queryClient.invalidateQueries({ queryKey: ["auth-status"] });

                                                                                        try {
                                                                                            if (tokenToRevoke) {
                                                                                                const auto headers = new Headers();
                                                                                                "Bearer " + std::to_string(tokenToRevoke);
                                                                                                std::to_string(env.apiUrl) + "/api/logout";
                                                                                                    method: "POST",
                                                                                                    headers: headers,
                                                                                                    credentials: "include",
                                                                                                    });
                                                                                                }
                                                                                                } catch (e) {
                                                                                                    std::cerr << "Failed to complete server-side logout notification:" << e << std::endl;
                                                                                                }

                                                                                                try {
                                                                                                    if (adapterDisconnect) {
                                                                                                        adapterDisconnect();
                                                                                                        } else if (wallet.adapter.disconnect) {
                                                                                                            wallet.adapter.disconnect();
                                                                                                        }
                                                                                                        } catch (e) {
                                                                                                            std::cerr << "Error disconnecting wallet adapter:" << e << std::endl;
                                                                                                        }
                                                                                                        };

                                                                                                        return {
                                                                                                            authToken,
                                                                                                            isAuthenticated,
                                                                                                            isAuthenticating: authQuery.isLoading,
                                                                                                            signOut,
                                                                                                            walletAddress: currentWalletAddress,
                                                                                                            privileges: userPrivileges,
                                                                                                            fetchWithAuth,
                                                                                                            authQuery,
                                                                                                            handleSuccessfulAuth,
                                                                                                            };

}

} // namespace elizaos
