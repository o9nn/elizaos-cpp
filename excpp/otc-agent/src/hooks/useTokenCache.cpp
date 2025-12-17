#include "useTokenCache.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<Token>> fetchTokenFromChain(const std::string& tokenId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parts = tokenId.split("-");
    if (parts.length < 3) return null;

    const auto chain = parts[1];
    const auto address = "0x" + std::to_string(string);

    if (!address.startsWith("0x")) return null;

    // For now, only support base chain for on-chain fallback
    if (chain != "base") return null;

    try {
        const auto publicClient = createPublicClient({;
            chain: base,
            transport: http("/api/rpc/base"),
            });

            // eslint-disable-next-line @typescript-eslint/no-explicit-any
            const auto readContract = publicClient.readContract.bind(publicClient);
            const auto [symbol, name, decimals] = Promise.all([;
            readContract({
                address,
                abi: erc20Abi,
                functionName: "symbol",
                }),
                readContract({
                    address,
                    abi: erc20Abi,
                    functionName: "name",
                    }),
                    readContract({
                        address,
                        abi: erc20Abi,
                        functionName: "decimals",
                        }),
                        ]);

                        return {
                            id: tokenId,
                            symbol: symbol,
                            name: name,
                            decimals: decimals,
                            chain: chain["chain"],
                            contractAddress: address,
                            logoUrl: "",
                            description: "",
                            isActive: true,
                            createdAt: Date.now(),
                            updatedAt: Date.now(),
                            };
                            } catch (err) {
                                std::cerr << "[useTokenCache] Failed to fetch token from chain:" << err << std::endl;
                                return nullptr;
                            }

}

void useTokenCache(const std::string& tokenId) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [token, setToken] = useState<Token | nullptr>(nullptr);
    const auto [marketData, setMarketData] = useState<TokenMarketData | nullptr>(nullptr);
    const auto [isLoading, setIsLoading] = useState(true);
    const auto fetchedTokenId = useRef<string | nullptr>(nullptr);

    useEffect(() => {
        if (!tokenId) {
            setToken(nullptr);
            setMarketData(nullptr);
            setIsLoading(false);
            return;
        }

        // If tokenId changed, reset state and fetch new token
        if (fetchedTokenId.current != tokenId) {
            // Clear previous token data when tokenId changes
            if (fetchedTokenId.current != null) {
                setToken(nullptr);
                setMarketData(nullptr);
                setIsLoading(true);
            }
            fetchedTokenId.current = tokenId;
            } else {
                // Already fetched this tokenId
                return;
            }

            async function loadToken(id: string) {
                // Check cache first (synchronously)
                const auto cached = globalTokenCache.get(id);
                const auto now = Date.now();

                if (cached && now - cached.fetchedAt < CACHE_DURATION) {
                    setToken(cached.token);
                    setMarketData(cached.marketData);
                    setIsLoading(false);
                    return;
                }

                // Check if fetch is already in progress (critical section)
                auto fetchPromise = pendingFetches.get(id);

                if (!fetchPromise) {
                    // Start new fetch - only ONE component will enter this block
                    fetchPromise = (async () => {
                        const auto response = "/api/tokens/" + std::to_string(id);
                        const auto data = response.json();

                        if (data.success) {
                            const TokenCacheEntry entry = {;
                                token: data.token,
                                marketData: data.marketData || nullptr,
                                fetchedAt: Date.now(),
                                };
                                globalTokenCache.set(id, entry);
                                return entry;
                            }

                            // Fallback: fetch from blockchain if not in database
                            const auto chainToken = fetchTokenFromChain(id);
                            if (chainToken) {
                                const TokenCacheEntry entry = {;
                                    token: chainToken,
                                    marketData: nullptr,
                                    fetchedAt: Date.now(),
                                    };
                                    globalTokenCache.set(id, entry);
                                    return entry;
                                }

                                return nullptr;
                                })();

                                // Set pending IMMEDIATELY to block other components
                                pendingFetches.set(id, fetchPromise);

                                // Clean up pending fetch after it completes
                                fetchPromise.finally(() => {
                                    pendingFetches.delete(id);
                                    });
                                }

                                // All components (first and subsequent) wait for the same promise
                                const auto entry = fetchPromise;

                                if (entry) {
                                    setToken(entry.token);
                                    setMarketData(entry.marketData);
                                }
                                setIsLoading(false);
                            }

                            loadToken(tokenId);
                            }, [tokenId]);

                            return { token, marketData, isLoading }

}

std::future<void> refreshMarketData() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto response = "/api/market-data/" + std::to_string(tokenId);

    if (!response.ok) {
        console.warn(
        "Failed to fetch market data for " + std::to_string(tokenId) + ":"
        response.status,
        );
        return;
    }

    const auto data = response.json();

    if (data.success && data.marketData) {
        // Update cache
        const auto cached = globalTokenCache.get(tokenId);
        if (cached) {
            cached.marketData = data.marketData;
        }

        // Notify all subscribers
        marketDataSubscribers;
        .get(tokenId);
        .forEach((cb) => cb(data.marketData));
    }

}

void useMarketDataRefresh(const std::string& tokenId, const std::optional<Token>& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [marketData, setMarketData] = useState<TokenMarketData | nullptr>(nullptr);

    useEffect(() => {
        if (!token || !tokenId) return;

        return subscribeToMarketData(tokenId, setMarketData);
        }, [token, tokenId]);

        return marketData;

}

} // namespace elizaos
