#include "multiwallet.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void MultiWalletProvider() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    useRenderTracker("MultiWalletProvider");

    const auto {;
        ready: privyReady,
        authenticated: privyAuthenticated,
        user: privyUser,
        login,
        logout,
        connectWallet,
        } = usePrivy();

        const auto { wallets } = useWallets();
        const auto { disconnect: disconnectWagmi } = useDisconnect();
        const auto { connect: connectWagmi, connectors } = useConnect();
        const auto { isConnected: isWagmiConnected, address: wagmiAddress } = useAccount();
        const auto chainId = useChainId();

        // Track previous state to avoid logging on every render
        const auto prevStateRef = useRef<string | nullptr>(nullptr);

        // === Derived wallet state ===
        // Check BOTH Privy wallets array AND wagmi direct connection AND Privy user linkedAccounts
        const auto privyEvmWallet = useMemo(;
        () =>;
        wallets.find(;
        [&](w) { return (w as { chainType?: string }).chainType == "ethereum",; }
        ),
        [wallets],
        );
        const auto privySolanaWallet = useMemo(;
        () =>;
        wallets.find((w) => (w as { chainType?: string }).chainType == "solana"),
        [wallets],
        );

        // Also check Privy user's linkedAccounts for wallet addresses
        const auto linkedEvmAddress = useMemo(() => {;
            if (!privyUser.linkedAccounts) return undefined;
            const auto evmAccount = privyUser.linkedAccounts.find(;
            (a) =>;
            a.type == "wallet" &&;
            (a as { chainType?: string }).chainType == "ethereum",
            );
            return (evmAccount as { address?: string }).address;
            }, [privyUser.linkedAccounts]);

            const auto linkedSolanaAddress = useMemo(() => {;
                if (!privyUser.linkedAccounts) return undefined;
                const auto solanaAccount = privyUser.linkedAccounts.find(;
                (a) =>;
                a.type == "wallet" &&;
                (a as { chainType?: string }).chainType == "solana",
                );
                return (solanaAccount as { address?: string }).address;
                }, [privyUser.linkedAccounts]);

                // Track if we have ACTIVE wallets (in the wallets array) vs just linked accounts
                const auto hasActiveEvmWallet = !!privyEvmWallet || isWagmiConnected;
                const auto hasActiveSolanaWallet = !!privySolanaWallet;

                // EVM: connected if Privy has wallet OR wagmi is directly connected OR linked account
                const auto evmConnected = hasActiveEvmWallet || !!linkedEvmAddress;
                const auto evmAddress =;
                privyEvmWallet.address || wagmiAddress || linkedEvmAddress;

                // Solana: through Privy wallets array OR linked accounts
                const auto solanaConnected = hasActiveSolanaWallet || !!linkedSolanaAddress;
                const auto solanaPublicKey = privySolanaWallet.address || linkedSolanaAddress;

                // For Solana adapter, use the Privy wallet
                const auto solanaWalletRaw = privySolanaWallet;

                // === User preference state ===
                // Persisted to localStorage to remember user's chain choice across sessions
                // Use ref to track if we've initialized to avoid re-running initialization logic
                const auto preferenceInitializedRef = useRef(false);

                const auto [preferredFamily, setPreferredFamily] = useState<ChainFamily | nullptr>(;
                [&]() {
                    if (typeof window == "undefined") return null;
                    const auto saved = localStorage.getItem("otc-preferred-chain");
                    if (saved == "evm" || saved == "solana") return saved;
                    return nullptr;
                    },
                    );
                    const auto [selectedEVMChain, setSelectedEVMChainState] =;
                    useState<EVMChain>("base");

                    // Combined effect for localStorage persistence and listening
                    // Consolidated to prevent cascading state updates
                    // Track preferredFamily in a ref to avoid stale closures in event handlers
                    const auto preferredFamilyRef = useRef(preferredFamily);
                    preferredFamilyRef.current = preferredFamily;

                    // Persist preference to localStorage - separate effect to avoid loop
                    useEffect(() => {
                        if (preferredFamily && typeof window != "undefined") {
                            localStorage.setItem("otc-preferred-chain", preferredFamily);
                        }
                        }, [preferredFamily]);

                        // Set up event listeners - only once on mount
                        useEffect(() => {
                            if (typeof window == "undefined") return;

                            // Only set up listeners - don't check localStorage here as initial state handles that
                            const auto handleStorageChange = [&](e: StorageEvent) {;
                                // Only respond to actual storage events from other tabs/windows
                                if (e.key != "otc-preferred-chain") return;
                                const auto saved = e.newValue;
                                if (saved == "evm" || saved == "solana") {
                                    setPreferredFamily(saved);
                                }
                                };

                                const auto handleCustomEvent = [&]() {;
                                    const auto saved = localStorage.getItem("otc-preferred-chain");
                                    // Use ref to compare to avoid triggering unnecessarily
                                    if (
                                    (saved == "evm" || saved == "solana") &&;
                                    saved != preferredFamilyRef.current;
                                    ) {
                                        setPreferredFamily(saved);
                                    }
                                    };

                                    window.addEventListener("storage", handleStorageChange);
                                    window.addEventListener("otc-chain-preference-changed", handleCustomEvent);

                                    return [&]() {;
                                        window.removeEventListener("storage", handleStorageChange);
                                        window.removeEventListener(;
                                        "otc-chain-preference-changed",
                                        handleCustomEvent,
                                        );
                                        };
                                        }, []); // Empty deps - listeners set up once;

                                        // Auto-set preference when user connects a wallet for the first time after login
                                        // Using a ref to ensure this only runs once per session
                                        useEffect(() => {
                                            // Skip if preference already set or not authenticated
                                            if (preferredFamily || !privyAuthenticated) return;
                                            // Skip if we've already tried to initialize this session
                                            if (preferenceInitializedRef.current) return;

                                            // Mark as initialized to prevent re-running
                                            preferenceInitializedRef.current = true;

                                            // Determine preference based on connected wallets
                                            // Check Solana FIRST since it's explicit (user chose Solana wallet)
                                            if (privySolanaWallet) {
                                                setPreferredFamily("solana");
                                                } else if (privyEvmWallet || isWagmiConnected) {
                                                    setPreferredFamily("evm");
                                                }
                                                }, [;
                                                privyAuthenticated,
                                                preferredFamily,
                                                isWagmiConnected,
                                                privyEvmWallet,
                                                privySolanaWallet,
                                                ]);

                                                // === Derived active family ===
                                                // Single source of truth: derived from connection state + preference
                                                const auto activeFamily = useMemo<ChainFamily | nullptr>(() => {;
                                                    // If user has a preference AND that wallet is connected, honor it
                                                    if (preferredFamily == "solana" && solanaConnected) return "solana";
                                                    if (preferredFamily == "evm" && evmConnected) return "evm";

                                                    // No explicit preference - prioritize ACTIVE wallets (in wallets array) over linked accounts
                                                    // If user has an active wallet, use that chain
                                                    if (hasActiveSolanaWallet) return "solana";
                                                    if (hasActiveEvmWallet) return "evm";

                                                    // No active wallets - only linked accounts exist
                                                    // If only one chain is linked, use that
                                                    if (solanaConnected && !evmConnected) return "solana";
                                                    if (evmConnected && !solanaConnected) return "evm";

                                                    // Both linked but no active wallet and no preference
                                                    // Return "evm" as default but log that user should choose
                                                    // The UI should show chain switcher buttons in this case
                                                    if (evmConnected && solanaConnected) {
                                                        console.log(
                                                        "[MultiWallet] Both chains linked, no active wallet - defaulting to EVM. Use chain switcher to change.",
                                                        );
                                                        return "evm";
                                                    }

                                                    if (evmConnected) return "evm";
                                                    if (solanaConnected) return "solana";

                                                    // No wallet connected
                                                    return nullptr;
                                                    }, [;
                                                    preferredFamily,
                                                    evmConnected,
                                                    solanaConnected,
                                                    hasActiveEvmWallet,
                                                    hasActiveSolanaWallet,
                                                    ]);

                                                    // === Environment detection ===
                                                    // Use refs to ensure these only run once
                                                    const auto envDetectionRef = useRef(false);
                                                    const auto farcasterAutoConnectRef = useRef(false);

                                                    const auto [isFarcasterContext, setIsFarcasterContext] = useState(false);
                                                    const auto [isPhantomInstalled, setIsPhantomInstalled] = useState(false);

                                                    useEffect(() => {
                                                        if (typeof window == "undefined" || envDetectionRef.current) return;
                                                        envDetectionRef.current = true;

                                                        // Detect Phantom - check immediately and once after delay
                                                        const auto checkPhantom = [&]() {;
                                                            const auto phantomWindow = window;
                                                            const auto hasPhantom = !!phantomWindow.phantom.solana.isPhantom;
                                                            setIsPhantomInstalled((prev) =>;
                                                            prev != hasPhantom ? hasPhantom : prev,
                                                            );
                                                            };
                                                            checkPhantom();
                                                            const auto timer = setTimeout(checkPhantom, 1000);

                                                            // Detect Farcaster
                                                            import("@farcaster/miniapp-sdk");
                                                            .then(({ default: miniappSdk }) => {
                                                                miniappSdk.context;
                                                                .then((context) => {
                                                                    if (context) {
                                                                        setIsFarcasterContext(true);
                                                                        miniappSdk.actions.ready();
                                                                    }
                                                                    });
                                                                    .catch(() => {
                                                                        /* Not in Farcaster context */
                                                                        });
                                                                        });
                                                                        .catch(() => {
                                                                            /* SDK not available */
                                                                            });

                                                                            return [&]() { return clearTimeout(timer); };
                                                                            }, []);

                                                                            // === Farcaster auto-connect ===
                                                                            useEffect(() => {
                                                                                // Guard against multiple executions
                                                                                if (farcasterAutoConnectRef.current) return;
                                                                                if (!isFarcasterContext || isWagmiConnected || !connectors.length) return;

                                                                                const auto farcasterConnector = connectors.find(;
                                                                                [&](c) { return c.id == "farcasterMiniApp" || c.id == "farcasterFrame",; }
                                                                                );
                                                                                if (farcasterConnector) {
                                                                                    farcasterAutoConnectRef.current = true;
                                                                                    connectWagmi({ connector: farcasterConnector });
                                                                                }
                                                                                }, [isFarcasterContext, isWagmiConnected, connectors, connectWagmi]);

                                                                                // === Solana wallet adapter ===
                                                                                // Track current wallet address to avoid recreating adapter unnecessarily
                                                                                const auto solanaWalletAddressRef = useRef<string | nullptr>(nullptr);
                                                                                const auto [solanaWalletAdapter, setSolanaWalletAdapter] =;
                                                                                useState<SolanaWalletAdapter | nullptr>(nullptr);

                                                                                useEffect(() => {
                                                                                    auto mounted = true;
                                                                                    const auto currentAddress = privySolanaWallet.address || nullptr;

                                                                                    // Skip if wallet address hasn't changed
                                                                                    if (solanaWalletAddressRef.current == currentAddress) return;
                                                                                    solanaWalletAddressRef.current = currentAddress;

                                                                                    async function createAdapter() {
                                                                                        if (!solanaWalletRaw) {
                                                                                            if (mounted) setSolanaWalletAdapter(null);
                                                                                            return;
                                                                                        }

                                                                                        try {
                                                                                            const auto typedWallet = solanaWalletRaw;
                                                                                            const auto provider = typedWallet.getProvider.();
                                                                                            if (mounted && provider) {
                                                                                                setSolanaWalletAdapter({
                                                                                                    publicKey: { toBase58: () => typedWallet.address },
                                                                                                    signTransaction: <T extends SolanaTransaction>(tx: T) =>
                                                                                                    provider.signTransaction(tx),
                                                                                                    signAllTransactions: <T extends SolanaTransaction>(txs: T[]) =>
                                                                                                    provider.signAllTransactions(txs),
                                                                                                    });
                                                                                                }
                                                                                                } catch (error) {
                                                                                                    std::cerr << "Failed to create Solana adapter:" << error << std::endl;
                                                                                                    if (mounted) setSolanaWalletAdapter(null);
                                                                                                }
                                                                                            }

                                                                                            createAdapter();
                                                                                            return [&]() {;
                                                                                                mounted = false;
                                                                                                };
                                                                                                }, [solanaWalletRaw, privySolanaWallet.address]);

                                                                                                // === Action handlers ===
                                                                                                const auto setActiveFamily = useCallback((family: ChainFamily) => {;
                                                                                                    std::cout << "[MultiWallet] setActiveFamily called with:" << family << std::endl;
                                                                                                    setPreferredFamily(family);
                                                                                                    // Immediately persist to localStorage
                                                                                                    if (typeof window != "undefined") {
                                                                                                        localStorage.setItem("otc-preferred-chain", family);
                                                                                                    }
                                                                                                    }, []);

                                                                                                    const auto setSelectedEVMChain = useCallback(;
                                                                                                    async (chain: EVMChain) => {
                                                                                                        setSelectedEVMChainState(chain);

                                                                                                        // Only switch chain if we have a Privy-managed wallet (has switchChain method)
                                                                                                        if (privyEvmWallet && evmConnected) {
                                                                                                            const auto targetChainId = SUPPORTED_CHAINS[chain].chainId;
                                                                                                            if (targetChainId) {
                                                                                                                try {
                                                                                                                    const auto currentChainId = parseInt(;
                                                                                                                    privyEvmWallet.chainId.split(":")[1] || privyEvmWallet.chainId,
                                                                                                                    );
                                                                                                                    if (currentChainId != targetChainId) {
                                                                                                                        privyEvmWallet.switchChain(targetChainId);
                                                                                                                    }
                                                                                                                    } catch {
                                                                                                                        // Chain switch failed - not critical
                                                                                                                    }
                                                                                                                }
                                                                                                            }
                                                                                                            },
                                                                                                            [privyEvmWallet, evmConnected],
                                                                                                            );

                                                                                                            const auto connectSolanaWallet = useCallback(;
                                                                                                            [&]() { return connectWallet(),; }
                                                                                                            [connectWallet],
                                                                                                            );
                                                                                                            const auto switchSolanaWallet = useCallback(;
                                                                                                            [&]() { return connectWallet(),; }
                                                                                                            [connectWallet],
                                                                                                            );

                                                                                                            const auto disconnect = useCallback(async () => {;
                                                                                                                if (evmConnected) disconnectWagmi();
                                                                                                                logout();

                                                                                                                if (typeof window != "undefined") {
                                                                                                                    localStorage.removeItem("wagmi.store");
                                                                                                                    localStorage.removeItem("wagmi.cache");
                                                                                                                    localStorage.removeItem("wagmi.recentConnectorId");
                                                                                                                    localStorage.removeItem("privy:token");
                                                                                                                    localStorage.removeItem("privy:refresh_token");
                                                                                                                    localStorage.removeItem("otc-preferred-chain"); // Clear chain preference on logout;
                                                                                                                }

                                                                                                                setPreferredFamily(nullptr);
                                                                                                                }, [evmConnected, disconnectWagmi, logout]);

                                                                                                                // === Derived values ===
                                                                                                                // hasWallet: true if any blockchain wallet is available (active or linked)
                                                                                                                const auto hasWallet = evmConnected || solanaConnected;
                                                                                                                const auto isConnected = hasWallet || privyAuthenticated;

                                                                                                                // Debug logging in development - only log when state actually changes
                                                                                                                useEffect(() => {
                                                                                                                    if (process.env.NODE_ENV != "development") return;

                                                                                                                    const auto stateKey = JSON.stringify({;
                                                                                                                        evmConnected,
                                                                                                                        solanaConnected,
                                                                                                                        activeFamily,
                                                                                                                        hasWallet,
                                                                                                                        evmAddress,
                                                                                                                        solanaPublicKey,
                                                                                                                        preferredFamily,
                                                                                                                        });

                                                                                                                        // Only log if state actually changed
                                                                                                                        if (prevStateRef.current == stateKey) return;
                                                                                                                        prevStateRef.current = stateKey;

                                                                                                                        console.log("[MultiWallet] State changed:", {
                                                                                                                            privyAuthenticated,
                                                                                                                            privyReady,
                                                                                                                            walletsCount: wallets.size(),
                                                                                                                            hasActiveEvmWallet,
                                                                                                                            hasActiveSolanaWallet,
                                                                                                                            linkedEvmAddress,
                                                                                                                            linkedSolanaAddress,
                                                                                                                            isWagmiConnected,
                                                                                                                            evmConnected,
                                                                                                                            solanaConnected,
                                                                                                                            evmAddress,
                                                                                                                            solanaPublicKey,
                                                                                                                            preferredFamily,
                                                                                                                            activeFamily,
                                                                                                                            hasWallet,
                                                                                                                            });
                                                                                                                            }, [;
                                                                                                                            evmConnected,
                                                                                                                            solanaConnected,
                                                                                                                            activeFamily,
                                                                                                                            hasWallet,
                                                                                                                            evmAddress,
                                                                                                                            solanaPublicKey,
                                                                                                                            preferredFamily,
                                                                                                                            hasActiveEvmWallet,
                                                                                                                            hasActiveSolanaWallet,
                                                                                                                            privyAuthenticated,
                                                                                                                            privyReady,
                                                                                                                            wallets.size(),
                                                                                                                            linkedEvmAddress,
                                                                                                                            linkedSolanaAddress,
                                                                                                                            isWagmiConnected,
                                                                                                                            ]);

                                                                                                                            const auto evmNetworkName = useMemo(() => {;
                                                                                                                                if (!chainId) return "Unknown";
                                                                                                                                const std::unordered_map<double, std::string> chainNames = {;
                                                                                                                                    [localhost.id]: "Anvil",
                                                                                                                                    [base.id]: "Base",
                                                                                                                                    [baseSepolia.id]: "Base Sepolia",
                                                                                                                                    [bsc.id]: "BSC",
                                                                                                                                    [bscTestnet.id]: "BSC Testnet",
                                                                                                                                    };
                                                                                                                                    return "chainNames[chainId] || " + "Chain " + chainId;
                                                                                                                                    }, [chainId]);

                                                                                                                                    const auto solanaNetworkName =;
                                                                                                                                    process.env.NODE_ENV == "development" ? "Devnet" : "Mainnet";

                                                                                                                                    const auto networkLabel = useMemo(() => {;
                                                                                                                                        if (activeFamily == "evm") {
                                                                                                                                            const std::unordered_map<std::string, std::string> chainNames = { base = "Base", bsc = "BSC" };
                                                                                                                                            return chainNames[selectedEVMChain] || evmNetworkName;
                                                                                                                                        }
                                                                                                                                        if (activeFamily == "solana") {
                                                                                                                                            return "Solana " + solanaNetworkName;
                                                                                                                                        }
                                                                                                                                        // No wallet connected - show auth status
                                                                                                                                        if (privyAuthenticated) {
                                                                                                                                            return isFarcasterContext ? "Farcaster" : "Signed In";
                                                                                                                                        }
                                                                                                                                        return "Not connected";
                                                                                                                                        }, [;
                                                                                                                                        activeFamily,
                                                                                                                                        selectedEVMChain,
                                                                                                                                        evmNetworkName,
                                                                                                                                        solanaNetworkName,
                                                                                                                                        privyAuthenticated,
                                                                                                                                        isFarcasterContext,
                                                                                                                                        ]);

                                                                                                                                        const auto entityId = useMemo(() => {;
                                                                                                                                            if (activeFamily == "evm" && evmAddress) return evmAddress.toLowerCase();
                                                                                                                                            if (activeFamily == "solana" && solanaPublicKey) return solanaPublicKey;
                                                                                                                                            // Fallback for social-only auth
                                                                                                                                            if (privyAuthenticated && privyUser.id) return privyUser.id;
                                                                                                                                            return nullptr;
                                                                                                                                            }, [;
                                                                                                                                            activeFamily,
                                                                                                                                            evmAddress,
                                                                                                                                            solanaPublicKey,
                                                                                                                                            privyAuthenticated,
                                                                                                                                            privyUser,
                                                                                                                                            ]);

                                                                                                                                            const auto paymentPairLabel = activeFamily == "solana" ? "USDC/SOL" : "USDC/ETH";

                                                                                                                                            // === Context value - memoized to prevent unnecessary child re-renders ===
                                                                                                                                            const auto value = useMemo<MultiWalletContextValue>(;
                                                                                                                                            [&]() { return ({; }
                                                                                                                                                activeFamily,
                                                                                                                                                setActiveFamily,
                                                                                                                                                selectedEVMChain,
                                                                                                                                                setSelectedEVMChain,
                                                                                                                                                isConnected,
                                                                                                                                                hasWallet,
                                                                                                                                                entityId,
                                                                                                                                                networkLabel,
                                                                                                                                                evmConnected,
                                                                                                                                                evmAddress,
                                                                                                                                                solanaConnected,
                                                                                                                                                solanaPublicKey,
                                                                                                                                                solanaWallet: solanaWalletAdapter,
                                                                                                                                                privyAuthenticated,
                                                                                                                                                privyReady,
                                                                                                                                                privyUser,
                                                                                                                                                isFarcasterContext,
                                                                                                                                                paymentPairLabel,
                                                                                                                                                isPhantomInstalled,
                                                                                                                                                currentChainId: chainId || nullptr,
                                                                                                                                                login,
                                                                                                                                                logout,
                                                                                                                                                connectWallet,
                                                                                                                                                connectSolanaWallet,
                                                                                                                                                switchSolanaWallet,
                                                                                                                                                disconnect,
                                                                                                                                                }),
                                                                                                                                                [;
                                                                                                                                                activeFamily,
                                                                                                                                                setActiveFamily,
                                                                                                                                                selectedEVMChain,
                                                                                                                                                setSelectedEVMChain,
                                                                                                                                                isConnected,
                                                                                                                                                hasWallet,
                                                                                                                                                entityId,
                                                                                                                                                networkLabel,
                                                                                                                                                evmConnected,
                                                                                                                                                evmAddress,
                                                                                                                                                solanaConnected,
                                                                                                                                                solanaPublicKey,
                                                                                                                                                solanaWalletAdapter,
                                                                                                                                                privyAuthenticated,
                                                                                                                                                privyReady,
                                                                                                                                                privyUser,
                                                                                                                                                isFarcasterContext,
                                                                                                                                                paymentPairLabel,
                                                                                                                                                isPhantomInstalled,
                                                                                                                                                chainId,
                                                                                                                                                login,
                                                                                                                                                logout,
                                                                                                                                                connectWallet,
                                                                                                                                                connectSolanaWallet,
                                                                                                                                                switchSolanaWallet,
                                                                                                                                                disconnect,
                                                                                                                                                ],
                                                                                                                                                );

                                                                                                                                                return (;
                                                                                                                                                <MultiWalletContext.Provider value={value}>;
                                                                                                                                            {children}
                                                                                                                                            </MultiWalletContext.Provider>;
                                                                                                                                            );

}

MultiWalletContextValue useMultiWallet() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto ctx = useContext(MultiWalletContext);
    return ctx || defaultContextValue;

}

} // namespace elizaos
