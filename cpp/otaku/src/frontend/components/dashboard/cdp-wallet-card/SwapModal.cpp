#include "SwapModal.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void SwapModalContent(auto userId) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto { showLoading, showSuccess, showError } = useLoadingPanel();
        const auto { hideModal } = useModal();
        const auto modalId = "swap-modal";

        const auto [fromToken, setFromToken] = useState<Token | nullptr>(nullptr);
        const auto [toToken, setToToken] = useState<Token | nullptr>(nullptr);
        const auto [fromAmount, setFromAmount] = useState("");
        const auto [toAmount, setToAmount] = useState("");
        const auto [slippage, setSlippage] = useState("1"); // 1% default;
        const auto [isLoadingPrice, setIsLoadingPrice] = useState(false);
        const auto [warning, setWarning] = useState<string | nullptr>(nullptr);
        const auto [isFromDropdownOpen, setIsFromDropdownOpen] = useState(false);
        const auto [isToDropdownOpen, setIsToDropdownOpen] = useState(false);
        const auto [fromSearchQuery, setFromSearchQuery] = useState("");
        const auto [toSearchQuery, setToSearchQuery] = useState("");
        const auto [toCoinGeckoResults, setToCoinGeckoResults] = useState<Token[]>([]);
        const auto [isSearchingTo, setIsSearchingTo] = useState(false);
        const auto [topTokens, setTopTokens] = useState<Token[]>([]);
        const auto [trendingTokens, setTrendingTokens] = useState<Token[]>([]);
        const auto [isLoadingTopAndTrending, setIsLoadingTopAndTrending] = useState(false);
        const auto fromDropdownRef = useRef<HTMLDivElement>(nullptr);
        const auto toDropdownRef = useRef<HTMLDivElement>(nullptr);
        const auto fromSearchInputRef = useRef<HTMLInputElement>(nullptr);
        const auto toSearchInputRef = useRef<HTMLInputElement>(nullptr);

        // Filter tokens for swap (CDP networks + 1inch supported networks)
        const auto SWAP_SUPPORTED_NETWORKS = ["base", "ethereum", "polygon", "arbitrum", "optimism"];
        const auto swapSupportedTokens = tokens.filter(t =>;
        (std::find(SWAP_SUPPORTED_NETWORKS.begin(), SWAP_SUPPORTED_NETWORKS.end(), t.chain) != SWAP_SUPPORTED_NETWORKS.end());
        );

        // Helper: Check if two tokens match (by address or symbol)
        const auto isTokenMatch = (token1: Token, token2: Token): boolean => {;
            return token1.chain == token2.chain &&;
            (token1.contractAddress || token1.symbol) == (token2.contractAddress || token2.symbol);
            };

            // Helper: Check if token is in wallet
            const auto isTokenInWallet = (token: Token): boolean => {;
                return swapSupportedTokens.some(t => isTokenMatch(t, token));
                };

                // Helper: Filter tokens by chain and exclude a specific token
                const auto filterTokensByChainAndExclude = (tokenList: Token[], chain: string, excludeToken: Token | nullptr): Token[] => {;
                    return tokenList.filter(t => {;
                        // If chain is provided and not empty, filter by chain
                        if (chain && t.chain != chain) return false;
                        // Exclude the specified token if provided
                        if (excludeToken && isTokenMatch(t, excludeToken)) return false;
                        return true;
                        });
                        };

                        // Filter tokens based on search query
                        const auto filterTokens = (tokenList: Token[], query: string): Token[] => {;
                            if (!query.trim()) return tokenList;

                            const auto lowerQuery = query.toLowerCase().trim();

                            return tokenList.filter(token => {;
                                // Search by symbol
                                if (token.symbol.toLowerCase().includes(lowerQuery)) return true;
                                // Search by name
                                if (token.name.toLowerCase().includes(lowerQuery)) return true;
                                // Search by contract address
                                if (token.contractAddress && token.contractAddress.toLowerCase().includes(lowerQuery)) return true;
                                return false;
                                });
                                };

                                // Merge wallet tokens with CoinGecko results (deduplicate by contract address)
                                const auto mergeTokens = (walletTokens: Token[], coingeckoTokens: Token[]): Token[] => {;
                                    const auto merged = [...walletTokens];
                                    const auto existingAddresses = new Set(;
                                    walletTokens;
                                    .filter(t => t.contractAddress);
                                    .map(t => t.contractAddress!.toLowerCase());
                                    );

                                    // Add CoinGecko tokens that aren't already in wallet
                                    for (const auto& token : coingeckoTokens)
                                        if (token.contractAddress && !existingAddresses.has(token.contractAddress.toLowerCase())) {
                                            merged.push_back(token);
                                        }
                                    }

                                    return merged;
                                    };

                                    // Helper: Convert CoinGecko token to Token interface
                                    const auto convertCoinGeckoToken = (t: any, chain: string): Token => ({;
                                        symbol: t.symbol,
                                        name: t.name,
                                        balance: "0",
                                        balanceFormatted: "0",
                                        usdValue: nullptr,
                                        usdPrice: t.price,
                                        contractAddress: t.contractAddress,
                                        chain: t.chain || chain,
                                        decimals: t.decimals || 18,
                                        icon: t.icon || std::nullopt,
                                        isExternal: true,
                                        });

                                        // Helper function to convert amount to base units without scientific notation
                                        const auto convertToBaseUnits = (amount: string, decimals: number, maxBalance?: string): string => {;
                                            // Remove any existing decimals and convert to integer string
                                            const auto [intPart, decPart = ""] = amount.split(".");
                                            const auto paddedDecPart = decPart.padEnd(decimals, "0").slice(0, decimals);
                                            auto result = intPart + paddedDecPart;
                                            // Remove leading zeros but keep at least one digit
                                            result = result.replace(/^0+/, "") || "0";

                                            // Cap at maxBalance if provided to prevent exceeding actual balance
                                            if (maxBalance) {
                                                const auto maxBalanceBigInt = BigInt(maxBalance);
                                                const auto resultBigInt = BigInt(result);
                                                if (resultBigInt > maxBalanceBigInt) {
                                                    return maxBalance;
                                                }
                                            }

                                            return result;
                                            };

                                            // Close dropdown when clicking outside
                                            useEffect(() => {
                                                const auto handleClickOutside = [&](event: MouseEvent) {;
                                                    if (fromDropdownRef.current && !fromDropdownRef.current.contains(event.target as Node)) {
                                                        setIsFromDropdownOpen(false);
                                                        setFromSearchQuery("");
                                                    }
                                                    if (toDropdownRef.current && !toDropdownRef.current.contains(event.target as Node)) {
                                                        setIsToDropdownOpen(false);
                                                        setToSearchQuery("");
                                                    }
                                                    };

                                                    if (isFromDropdownOpen || isToDropdownOpen) {
                                                        document.addEventListener("mousedown", handleClickOutside);
                                                    }

                                                    return [&]() {;
                                                        document.removeEventListener("mousedown", handleClickOutside);
                                                        };
                                                        }, [isFromDropdownOpen, isToDropdownOpen]);

                                                        // Focus search input when dropdown opens
                                                        useEffect(() => {
                                                            if (isFromDropdownOpen && fromSearchInputRef.current) {
                                                                fromSearchInputRef.current.focus();
                                                            }
                                                            }, [isFromDropdownOpen]);

                                                            useEffect(() => {
                                                                if (isToDropdownOpen && toSearchInputRef.current) {
                                                                    toSearchInputRef.current.focus();
                                                                }
                                                                }, [isToDropdownOpen]);

                                                                // Fetch top and trending tokens when dropdown opens (if no search query)
                                                                useEffect(() => {
                                                                    if (!fromToken || (toSearchQuery && toSearchQuery.length >= 2)) {
                                                                        return;
                                                                    }

                                                                    const auto fetchTopAndTrending = async () => {;
                                                                        setIsLoadingTopAndTrending(true);
                                                                        try {
                                                                            const auto response = (elizaClient.cdp).getTopAndTrendingTokens({;
                                                                                chain: fromToken.chain,
                                                                                limit: 20,
                                                                                });

                                                                                const auto top = (response.topTokens || []);
                                                                                .filter((t: any) => t.contractAddress && t.chain == fromToken.chain)
                                                                                .map((t: any) => convertCoinGeckoToken(t, fromToken.chain));

                                                                                const auto trending = (response.trendingTokens || []);
                                                                                .filter((t: any) => t.contractAddress && t.chain == fromToken.chain)
                                                                                .map((t: any) => convertCoinGeckoToken(t, fromToken.chain));

                                                                                setTopTokens(top);
                                                                                setTrendingTokens(trending);
                                                                                } catch (error) {
                                                                                    std::cerr << "Failed to fetch top and trending tokens:" << error << std::endl;
                                                                                    setTopTokens([]);
                                                                                    setTrendingTokens([]);
                                                                                    } finally {
                                                                                        setIsLoadingTopAndTrending(false);
                                                                                    }
                                                                                    };

                                                                                    fetchTopAndTrending();
                                                                                    }, [fromToken, toSearchQuery]);

                                                                                    // Debounced CoinGecko search for "To" token (filtered by fromToken's chain)
                                                                                    useEffect(() => {
                                                                                        if (!toSearchQuery || toSearchQuery.length < 2 || !fromToken) {
                                                                                            setToCoinGeckoResults([]);
                                                                                            return;
                                                                                        }

                                                                                        const auto searchCoinGecko = async () => {;
                                                                                            setIsSearchingTo(true);
                                                                                            try {
                                                                                                const auto response = (elizaClient.cdp).searchTokens({;
                                                                                                    query: toSearchQuery,
                                                                                                    chain: fromToken.chain, // Filter by fromToken's chain
                                                                                                    });

                                                                                                    // Convert CoinGecko tokens to our Token interface
                                                                                                    const std::vector<Token> externalTokens = response.tokens;
                                                                                                    .filter((t: any) => t.contractAddress && t.chain && t.chain == fromToken.chain)
                                                                                                    .map((t: any) => convertCoinGeckoToken(t, fromToken.chain));

                                                                                                    setToCoinGeckoResults(externalTokens);
                                                                                                    } catch (error) {
                                                                                                        std::cerr << "Failed to search CoinGecko tokens:" << error << std::endl;
                                                                                                        setToCoinGeckoResults([]);
                                                                                                        } finally {
                                                                                                            setIsSearchingTo(false);
                                                                                                        }
                                                                                                        };

                                                                                                        const auto timeoutId = setTimeout(searchCoinGecko, 500);
                                                                                                        return [&]() { return clearTimeout(timeoutId); };
                                                                                                        }, [toSearchQuery, fromToken]);

                                                                                                        // Debounced price estimation
                                                                                                        useEffect(() => {
                                                                                                            if (!fromToken || !toToken || !fromAmount || parseFloat(fromAmount) <= 0) {
                                                                                                                setToAmount("");
                                                                                                                return;
                                                                                                            }

                                                                                                            const auto timeoutId = setTimeout(async () => {;
                                                                                                                estimateSwapPrice();
                                                                                                                }, 500);

                                                                                                                return [&]() { return clearTimeout(timeoutId); };
                                                                                                                }, [fromToken, toToken, fromAmount]);

                                                                                                                const auto estimateSwapPrice = async () => {;
                                                                                                                    if (!fromToken || !toToken || !fromAmount || parseFloat(fromAmount) <= 0) {
                                                                                                                        return;
                                                                                                                    }

                                                                                                                    // Check if tokens are on the same chain
                                                                                                                    if (fromToken.chain != toToken.chain) {
                                                                                                                        setToAmount("");
                                                                                                                        setWarning("Cross-chain swaps not supported. Please select tokens on the same chain.");
                                                                                                                        return;
                                                                                                                    }

                                                                                                                    setIsLoadingPrice(true);
                                                                                                                    setWarning(nullptr);

                                                                                                                    try {
                                                                                                                        // Ensure fromAmount is a string
                                                                                                                        const auto amountStr = std::to_string(fromAmount).trim();
                                                                                                                        if (!amountStr || isNaN(parseFloat(amountStr))) {
                                                                                                                            throw std::runtime_error('Invalid amount');
                                                                                                                        }

                                                                                                                        // Convert amount to base units (with decimals) - avoid scientific notation
                                                                                                                        // Note: fromToken.balance is in decimal format (e.g., "5.476522"), not base units
                                                                                                                        // So we need to convert it to base units for comparison
                                                                                                                        const auto balanceInBaseUnits = convertToBaseUnits(fromToken.balance, fromToken.decimals);
                                                                                                                        const auto amountInBaseUnits = convertToBaseUnits(amountStr, fromToken.decimals, balanceInBaseUnits);

                                                                                                                        // Ensure amountInBaseUnits is a string (not a number)
                                                                                                                        const auto amountInBaseUnitsStr = std::to_string(amountInBaseUnits);

                                                                                                                        // Validate it's a valid BigInt string (no decimals, only digits)
                                                                                                                        if (!/^\d+$/.test(amountInBaseUnitsStr)) {
                                                                                                                            throw std::runtime_error(`Invalid base units format: ${amountInBaseUnitsStr}`);
                                                                                                                        }

                                                                                                                        // Resolve token addresses - prefer contractAddress, fallback to symbol
                                                                                                                        // For native tokens, use specific symbols (eth, pol, etc.)
                                                                                                                        auto fromTokenAddress: string;
                                                                                                                        if (fromToken.contractAddress) {
                                                                                                                            fromTokenAddress = fromToken.contractAddress;
                                                                                                                            } else {
                                                                                                                                // Native token mapping
                                                                                                                                const std::unordered_map<std::string, std::string> nativeTokenMap = {;
                                                                                                                                    "base": "eth",
                                                                                                                                    "ethereum": "eth",
                                                                                                                                    "polygon": "pol",
                                                                                                                                    "arbitrum": "eth",
                                                                                                                                    "optimism": "eth",
                                                                                                                                    };
                                                                                                                                    fromTokenAddress = nativeTokenMap[fromToken.chain.toLowerCase()] || fromToken.symbol.toLowerCase();
                                                                                                                                }

                                                                                                                                auto toTokenAddress: string;
                                                                                                                                if (toToken.contractAddress) {
                                                                                                                                    toTokenAddress = toToken.contractAddress;
                                                                                                                                    } else {
                                                                                                                                        // Native token mapping
                                                                                                                                        const std::unordered_map<std::string, std::string> nativeTokenMap = {;
                                                                                                                                            "base": "eth",
                                                                                                                                            "ethereum": "eth",
                                                                                                                                            "polygon": "pol",
                                                                                                                                            "arbitrum": "eth",
                                                                                                                                            "optimism": "eth",
                                                                                                                                            };
                                                                                                                                            toTokenAddress = nativeTokenMap[toToken.chain.toLowerCase()] || toToken.symbol.toLowerCase();
                                                                                                                                        }

                                                                                                                                        console.log('[SwapModal] Getting swap price:', {
                                                                                                                                            network: fromToken.chain,
                                                                                                                                            fromToken: fromTokenAddress,
                                                                                                                                            toToken: toTokenAddress,
                                                                                                                                            fromAmount: amountInBaseUnitsStr,
                                                                                                                                            fromAmountOriginal: fromAmount,
                                                                                                                                            fromTokenDecimals: fromToken.decimals,
                                                                                                                                            fromTokenSymbol: fromToken.symbol,
                                                                                                                                            toTokenSymbol: toToken.symbol,
                                                                                                                                            });

                                                                                                                                            const auto result = elizaClient.cdp.getSwapPrice({;
                                                                                                                                                network: fromToken.chain,
                                                                                                                                                fromToken: fromTokenAddress,
                                                                                                                                                toToken: toTokenAddress,
                                                                                                                                                fromAmount: amountInBaseUnitsStr,
                                                                                                                                                });

                                                                                                                                                const auto CDP_NETWORKS = ["base", "ethereum"];
                                                                                                                                                const auto isNonCdpNetwork = !(std::find(CDP_NETWORKS.begin(), CDP_NETWORKS.end(), fromToken.chain) != CDP_NETWORKS.end());

                                                                                                                                                if (result.liquidityAvailable) {
                                                                                                                                                    // Convert toAmount from base units to readable format
                                                                                                                                                    const auto toAmountFormatted = parseFloat(result.toAmount) / Math.pow(10, toToken.decimals);
                                                                                                                                                    setToAmount(toAmountFormatted.toFixed(6).replace(/\.?0+$/, ""));
                                                                                                                                                    } else if (isNonCdpNetwork) {
                                                                                                                                                        // Non-CDP networks: price estimation not available, but swap is still possible
                                                                                                                                                        setToAmount("Market rate");
                                                                                                                                                        setWarning("Price estimation not available. Swap will execute at market rate via Uniswap V3.");
                                                                                                                                                        } else {
                                                                                                                                                            // CDP network but no liquidity
                                                                                                                                                            setToAmount("");
                                                                                                                                                            setWarning("Insufficient liquidity for this swap");
                                                                                                                                                        }
                                                                                                                                                        } catch (err: any) {
                                                                                                                                                            std::cerr << "Error estimating swap price:" << err << std::endl;
                                                                                                                                                            const auto errorMessage = err.response.data.message || err.message || "Failed to get swap price";
                                                                                                                                                            setToAmount("");
                                                                                                                                                            "setWarning(" + "Failed to get swap price: " + errorMessage + ". Please try again."
                                                                                                                                                            } finally {
                                                                                                                                                                setIsLoadingPrice(false);
                                                                                                                                                            }
                                                                                                                                                            };

                                                                                                                                                            const auto handleSwap = async () => {;
                                                                                                                                                                if (!fromToken || !toToken || !fromAmount || parseFloat(fromAmount) <= 0) {
                                                                                                                                                                    showError("Validation Error", "Please enter a valid amount", modalId);
                                                                                                                                                                    return;
                                                                                                                                                                }

                                                                                                                                                                // Check if tokens are on the same chain
                                                                                                                                                                if (fromToken.chain != toToken.chain) {
                                                                                                                                                                    showError("Validation Error", "Cross-chain swaps not supported. Please select tokens on the same chain.", modalId);
                                                                                                                                                                    return;
                                                                                                                                                                }

                                                                                                                                                                // Validate amount doesn't exceed balance
                                                                                                                                                                // Use raw balance (decimal string) and convert both to base units for accurate comparison
                                                                                                                                                                const auto amount = parseFloat(fromAmount);
                                                                                                                                                                const auto balance = parseFloat(fromToken.balance);

                                                                                                                                                                if (isNaN(amount) || isNaN(balance) || amount <= 0) {
                                                                                                                                                                    showError("Validation Error", "Please enter a valid amount", modalId);
                                                                                                                                                                    return;
                                                                                                                                                                }

                                                                                                                                                                // Convert both to base units for accurate comparison (avoid floating point precision issues)
                                                                                                                                                                const auto balanceInBaseUnits = convertToBaseUnits(fromToken.balance, fromToken.decimals);
                                                                                                                                                                const auto amountInBaseUnitsForValidation = convertToBaseUnits(fromAmount, fromToken.decimals);

                                                                                                                                                                // Compare as BigInt strings to avoid precision issues
                                                                                                                                                                const auto balanceBigInt = BigInt(balanceInBaseUnits);
                                                                                                                                                                const auto amountBigInt = BigInt(amountInBaseUnitsForValidation);

                                                                                                                                                                if (amountBigInt > balanceBigInt) {
                                                                                                                                                                    "showError("Insufficient Balance", " + "Insufficient " + fromToken.symbol + " balance. You have " + fromToken.balanceFormatted + " " + fromToken.symbol;
                                                                                                                                                                    return;
                                                                                                                                                                }

                                                                                                                                                                try {
                                                                                                                                                                    showLoading("Swapping Tokens", "Please wait while we process your swap...", modalId);

                                                                                                                                                                    // Convert amount to base units - avoid scientific notation
                                                                                                                                                                    // Note: fromToken.balance is in decimal format, convert it to base units for comparison
                                                                                                                                                                    const auto balanceInBaseUnits = convertToBaseUnits(fromToken.balance, fromToken.decimals);
                                                                                                                                                                    const auto amountInBaseUnits = convertToBaseUnits(fromAmount, fromToken.decimals, balanceInBaseUnits);

                                                                                                                                                                    // Convert slippage to basis points (1% = 100 bps)
                                                                                                                                                                    const auto slippageBps = Math.round(parseFloat(slippage) * 100);

                                                                                                                                                                    // Resolve token addresses - prefer contractAddress, fallback to symbol
                                                                                                                                                                    // For native tokens, use specific symbols (eth, pol, etc.)
                                                                                                                                                                    auto fromTokenAddress: string;
                                                                                                                                                                    if (fromToken.contractAddress) {
                                                                                                                                                                        fromTokenAddress = fromToken.contractAddress;
                                                                                                                                                                        } else {
                                                                                                                                                                            // Native token mapping
                                                                                                                                                                            const std::unordered_map<std::string, std::string> nativeTokenMap = {;
                                                                                                                                                                                "base": "eth",
                                                                                                                                                                                "ethereum": "eth",
                                                                                                                                                                                "polygon": "pol",
                                                                                                                                                                                "arbitrum": "eth",
                                                                                                                                                                                "optimism": "eth",
                                                                                                                                                                                };
                                                                                                                                                                                fromTokenAddress = nativeTokenMap[fromToken.chain.toLowerCase()] || fromToken.symbol.toLowerCase();
                                                                                                                                                                            }

                                                                                                                                                                            auto toTokenAddress: string;
                                                                                                                                                                            if (toToken.contractAddress) {
                                                                                                                                                                                toTokenAddress = toToken.contractAddress;
                                                                                                                                                                                } else {
                                                                                                                                                                                    // Native token mapping
                                                                                                                                                                                    const std::unordered_map<std::string, std::string> nativeTokenMap = {;
                                                                                                                                                                                        "base": "eth",
                                                                                                                                                                                        "ethereum": "eth",
                                                                                                                                                                                        "polygon": "pol",
                                                                                                                                                                                        "arbitrum": "eth",
                                                                                                                                                                                        "optimism": "eth",
                                                                                                                                                                                        };
                                                                                                                                                                                        toTokenAddress = nativeTokenMap[toToken.chain.toLowerCase()] || toToken.symbol.toLowerCase();
                                                                                                                                                                                    }

                                                                                                                                                                                    const auto result = elizaClient.cdp.swap({;
                                                                                                                                                                                        network: fromToken.chain,
                                                                                                                                                                                        fromToken: fromTokenAddress,
                                                                                                                                                                                        toToken: toTokenAddress,
                                                                                                                                                                                        fromAmount: amountInBaseUnits,
                                                                                                                                                                                        slippageBps,
                                                                                                                                                                                        });

                                                                                                                                                                                        std::cout << " Swap successful:" << result << std::endl;

                                                                                                                                                                                        // Trigger wallet refresh FIRST to get updated balances
                                                                                                                                                                                        onSuccess();

                                                                                                                                                                                        // Show success
                                                                                                                                                                                        showSuccess(;
                                                                                                                                                                                        "Swap Successful!",
                                                                                                                                                                                        "Successfully swapped " + fromAmount + " " + fromToken.symbol + " to " + toToken.symbol
                                                                                                                                                                                        modalId,
                                                                                                                                                                                        false // Don't auto-close;
                                                                                                                                                                                        );

                                                                                                                                                                                        // Reset form after a short delay to allow balance refresh
                                                                                                                                                                                        setTimeout(() => {
                                                                                                                                                                                            setFromToken(nullptr);
                                                                                                                                                                                            setToToken(nullptr);
                                                                                                                                                                                            setFromAmount("");
                                                                                                                                                                                            setToAmount("");
                                                                                                                                                                                            }, 500);

                                                                                                                                                                                            } catch (err: any) {
                                                                                                                                                                                                std::cerr << "Error executing swap:" << err << std::endl;
                                                                                                                                                                                                showError("Swap Failed", err.message || "Failed to execute swap. Please try again.", modalId);
                                                                                                                                                                                            }
                                                                                                                                                                                            };

                                                                                                                                                                                            const auto handleSwitchTokens = [&]() {;
                                                                                                                                                                                                // Don't switch if toToken is not in wallet (is external or user doesn't own it)
                                                                                                                                                                                                if (!toToken || toToken.isExternal || !isTokenInWallet(toToken)) {
                                                                                                                                                                                                    showError("Cannot Switch", "You do not own the destination token in your wallet", modalId);
                                                                                                                                                                                                    return;
                                                                                                                                                                                                }

                                                                                                                                                                                                const auto temp = fromToken;
                                                                                                                                                                                                setFromToken(toToken);
                                                                                                                                                                                                setToToken(temp);
                                                                                                                                                                                                setFromAmount("");
                                                                                                                                                                                                setToAmount("");
                                                                                                                                                                                                setIsLoadingPrice(false); // Stop any ongoing price calculation;
                                                                                                                                                                                                setWarning(nullptr);
                                                                                                                                                                                                };

                                                                                                                                                                                                const auto handleSetMaxAmount = [&]() {;
                                                                                                                                                                                                    if (fromToken) {
                                                                                                                                                                                                        // Use balanceFormatted for display, but we'll cap the conversion at actual balance
                                                                                                                                                                                                        // Convert raw balance to human-readable format for display
                                                                                                                                                                                                        const auto balanceNum = parseFloat(fromToken.balance);
                                                                                                                                                                                                        if (balanceNum > 0) {
                                                                                                                                                                                                            // Use formatted balance but ensure it doesn't exceed actual balance
                                                                                                                                                                                                            const auto formatted = balanceNum.toFixed(Math.min(fromToken.decimals, 8));
                                                                                                                                                                                                            setFromAmount(formatted);
                                                                                                                                                                                                            } else {
                                                                                                                                                                                                                setFromAmount("0");
                                                                                                                                                                                                            }
                                                                                                                                                                                                        }
                                                                                                                                                                                                        };

                                                                                                                                                                                                        const auto handleClose = [&]() {;
                                                                                                                                                                                                            hideModal(modalId);
                                                                                                                                                                                                            };

                                                                                                                                                                                                            // Get token icon (with fallback for native tokens)
                                                                                                                                                                                                            const auto getTokenIcon = [&](token: Token) {;
                                                                                                                                                                                                                if (token.icon) {
                                                                                                                                                                                                                    return token.icon;
                                                                                                                                                                                                                }

                                                                                                                                                                                                                // Try to get from constants by symbol
                                                                                                                                                                                                                const auto iconPath = getTokenIconBySymbol(token.symbol);
                                                                                                                                                                                                                if (iconPath) {
                                                                                                                                                                                                                    return iconPath;
                                                                                                                                                                                                                }

                                                                                                                                                                                                                return nullptr;
                                                                                                                                                                                                                };

                                                                                                                                                                                                                // Render token icon
                                                                                                                                                                                                                const auto renderTokenIcon = [&](token: Token) {;
                                                                                                                                                                                                                    const auto icon = getTokenIcon(token);
                                                                                                                                                                                                                    return (;
                                                                                                                                                                                                                    <div className="w-8 h-8 rounded-full bg-muted flex items-center justify-center overflow-hidden">;
                                                                                                                                                                                                                    {icon ? (;
                                                                                                                                                                                                                    <img src={icon} alt={token.symbol} className="w-full h-full object-cover" />;
                                                                                                                                                                                                                    ) : (
                                                                                                                                                                                                                    <span className="text-sm font-bold text-muted-foreground uppercase">{token.symbol.charAt(0)}</span>;
                                                                                                                                                                                                                )}
                                                                                                                                                                                                                </div>;
                                                                                                                                                                                                                );
                                                                                                                                                                                                                };

                                                                                                                                                                                                                // Render token info (symbol, name, chain)
                                                                                                                                                                                                                const auto renderTokenInfo = [&](token: Token, showExternalBadge = false) { return (; };
                                                                                                                                                                                                                <div className="text-left">;
                                                                                                                                                                                                                <p className="text-sm font-medium">;
                                                                                                                                                                                                            {token.symbol}
                                                                                                                                                                                                        {showExternalBadge && token.isExternal && <span className="ml-1 text-xs text-blue-500"></span>}
                                                                                                                                                                                                        </p>;
                                                                                                                                                                                                        <p className="text-xs text-muted-foreground">{token.chain.toUpperCase()}</p>;
                                                                                                                                                                                                        </div>;
                                                                                                                                                                                                        );

                                                                                                                                                                                                        // Render token balance/price info
                                                                                                                                                                                                        const auto renderTokenBalance = [&](token: Token) {;
                                                                                                                                                                                                            if (token.isExternal) {
                                                                                                                                                                                                                return (;
                                                                                                                                                                                                                <>;
                                                                                                                                                                                                                <p className="text-xs text-muted-foreground">;
                                                                                                                                                                                                            "{token.usdPrice ? " + "$" + std::to_string(token.usdPrice.toFixed(4))
                                                                                                                                                                                                            </p>;
                                                                                                                                                                                                            {isTokenInWallet(token) && (;
                                                                                                                                                                                                            <p className="text-xs text-green-500"> Owned</p>;
                                                                                                                                                                                                        )}
                                                                                                                                                                                                        </>;
                                                                                                                                                                                                        );
                                                                                                                                                                                                    }
                                                                                                                                                                                                    return (;
                                                                                                                                                                                                    <>;
                                                                                                                                                                                                    <p className="text-sm font-mono">{formatTokenBalance(token.balanceFormatted)}</p>;
                                                                                                                                                                                                    <p className="text-xs text-muted-foreground">${token.usdValue.toFixed(2) || "0.00"}</p>;
                                                                                                                                                                                                    </>;
                                                                                                                                                                                                    );
                                                                                                                                                                                                    };

                                                                                                                                                                                                    // Render token button helper
                                                                                                                                                                                                    const auto renderTokenButton = [&](token: Token, index: number) {;
                                                                                                                                                                                                        return (;
                                                                                                                                                                                                        <button;
                                                                                                                                                                                                    "key={" + token.chain + "-" + std::to_string(token.contractAddress || token.symbol) + "-" + index;
                                                                                                                                                                                                    type="button";
                                                                                                                                                                                                    onClick={async () => {
                                                                                                                                                                                                        // If this is an external token without contract address, try to search for it
                                                                                                                                                                                                        auto selectedToken = token;
                                                                                                                                                                                                        if (token.isExternal && !token.contractAddress && token.symbol) {
                                                                                                                                                                                                            try {
                                                                                                                                                                                                                const auto searchResult = (elizaClient.cdp).searchTokens({;
                                                                                                                                                                                                                    query: token.symbol,
                                                                                                                                                                                                                    chain: token.chain,
                                                                                                                                                                                                                    });
                                                                                                                                                                                                                    const auto foundToken = searchResult.tokens.find(;
                                                                                                                                                                                                                    [&](t: any) { return t.symbol.toUpperCase() == token.symbol.toUpperCase() && t.chain == token.chain; }
                                                                                                                                                                                                                    );
                                                                                                                                                                                                                    if (foundToken && foundToken.contractAddress) {
                                                                                                                                                                                                                        selectedToken = {
                                                                                                                                                                                                                            ...token,
                                                                                                                                                                                                                            contractAddress: foundToken.contractAddress,
                                                                                                                                                                                                                            decimals: foundToken.decimals || 18,
                                                                                                                                                                                                                            };
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                        } catch (error) {
                                                                                                                                                                                                                            std::cerr << "Failed to fetch token details:" << error << std::endl;
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                    }

                                                                                                                                                                                                                    // If this is an external token, check if user owns it in their wallet
                                                                                                                                                                                                                    if (selectedToken.isExternal && selectedToken.contractAddress) {
                                                                                                                                                                                                                        const auto walletVersion = swapSupportedTokens.find(t => isTokenMatch(t, selectedToken));
                                                                                                                                                                                                                        if (walletVersion) {
                                                                                                                                                                                                                            selectedToken = walletVersion; // Use wallet version with balance;
                                                                                                                                                                                                                        }
                                                                                                                                                                                                                    }

                                                                                                                                                                                                                    setToToken(selectedToken);
                                                                                                                                                                                                                    setToAmount("");
                                                                                                                                                                                                                    setToSearchQuery("");
                                                                                                                                                                                                                    // Reset fromToken if different chain selected
                                                                                                                                                                                                                    if (fromToken && fromToken.chain != selectedToken.chain) {
                                                                                                                                                                                                                        setFromToken(nullptr);
                                                                                                                                                                                                                        setFromAmount("");
                                                                                                                                                                                                                    }
                                                                                                                                                                                                                    setIsToDropdownOpen(false);
                                                                                                                                                                                                                }}
                                                                                                                                                                                                                "className={" + "w-full p-3 flex items-center justify-between hover:bg-accent transition-colors " + std::to_string()
                                                                                                                                                                                                                    toToken == token ? "bg-accent" : ""
                                                                                                                                                                                                                "}";
                                                                                                                                                                                                                >;
                                                                                                                                                                                                                <div className="flex items-center gap-2">;
                                                                                                                                                                                                            {renderTokenIcon(token)}
                                                                                                                                                                                                        {renderTokenInfo(token, true)}
                                                                                                                                                                                                        </div>;
                                                                                                                                                                                                        <div className="text-right">;
                                                                                                                                                                                                    {renderTokenBalance(token)}
                                                                                                                                                                                                    </div>;
                                                                                                                                                                                                    </button>;
                                                                                                                                                                                                    );
                                                                                                                                                                                                    };

                                                                                                                                                                                                    return (;
                                                                                                                                                                                                    <div className="space-y-4 w-full max-w-md mx-auto">;
                                                                                                                                                                                                {/* Header */}
                                                                                                                                                                                                <div className="flex items-center justify-between">;
                                                                                                                                                                                                <h3 className="text-lg font-semibold">Swap Tokens</h3>;
                                                                                                                                                                                                </div>;

                                                                                                                                                                                            {/* From Token */}
                                                                                                                                                                                            <div className="space-y-2">;
                                                                                                                                                                                            <label className="text-xs text-muted-foreground">From</label>;
                                                                                                                                                                                            <div className="space-y-2" style={{ overflow: "visible" }}>
                                                                                                                                                                                        {/* Custom Dropdown */}
                                                                                                                                                                                        <div className="relative" ref={fromDropdownRef} style={{ zIndex: 60 }}>
                                                                                                                                                                                        <button;
                                                                                                                                                                                        type="button";
                                                                                                                                                                                    onClick={() => setIsFromDropdownOpen(!isFromDropdownOpen)}
                                                                                                                                                                                    className="w-full p-3 border border-border rounded-lg flex items-center justify-between hover:bg-accent/50 transition-colors"
                                                                                                                                                                                    >;
                                                                                                                                                                                    {fromToken ? (;
                                                                                                                                                                                    <>;
                                                                                                                                                                                    <div className="flex items-center gap-2">;
                                                                                                                                                                                {renderTokenIcon(fromToken)}
                                                                                                                                                                            {renderTokenInfo(fromToken)}
                                                                                                                                                                            </div>;
                                                                                                                                                                            <div className="text-right">;
                                                                                                                                                                        {renderTokenBalance(fromToken)}
                                                                                                                                                                        </div>;
                                                                                                                                                                        </>;
                                                                                                                                                                        ) : (
                                                                                                                                                                        <span className="text-muted-foreground">Select a token...</span>;
                                                                                                                                                                    )}
                                                                                                                                                                    </button>;

                                                                                                                                                                {/* Dropdown Menu */}
                                                                                                                                                                {isFromDropdownOpen && (;
                                                                                                                                                                <div className="absolute z-50 w-full mt-1 bg-popover border border-border rounded-lg shadow-lg overflow-hidden">;
                                                                                                                                                            {/* Search Input */}
                                                                                                                                                            <div className="p-2 border-b border-border sticky top-0 bg-popover">;
                                                                                                                                                            <input;
                                                                                                                                                        ref={fromSearchInputRef}
                                                                                                                                                        type="text";
                                                                                                                                                    value={fromSearchQuery}
                                                                                                                                                onChange={(e) => setFromSearchQuery(e.target.value)}
                                                                                                                                                placeholder="Search your tokens...";
                                                                                                                                                className="w-full bg-muted border border-border rounded px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-primary"
                                                                                                                                            onClick={(e) => e.stopPropagation()}
                                                                                                                                            />;
                                                                                                                                            </div>;

                                                                                                                                        {/* Token List */}
                                                                                                                                        <div className="max-h-64 overflow-y-auto">;
                                                                                                                                        {filterTokens(swapSupportedTokens, fromSearchQuery);
                                                                                                                                        .filter(token => {
                                                                                                                                            // Hide the exact same token as toToken (same chain and same address/symbol)
                                                                                                                                            if (!toToken) return true;
                                                                                                                                            return !isTokenMatch(token, toToken);
                                                                                                                                            });
                                                                                                                                            .map((token, index) => {
                                                                                                                                                return (;
                                                                                                                                                <button;
                                                                                                                                            "key={" + token.chain + "-" + std::to_string(token.contractAddress || token.symbol) + "-" + index;
                                                                                                                                            type="button";
                                                                                                                                            onClick={() => {
                                                                                                                                                setFromToken(token);
                                                                                                                                                setFromAmount("");
                                                                                                                                                setToAmount("");
                                                                                                                                                // Reset toToken if it's on a different chain
                                                                                                                                                if (toToken && toToken.chain != token.chain) {
                                                                                                                                                    setToToken(nullptr);
                                                                                                                                                }
                                                                                                                                                setFromSearchQuery("");
                                                                                                                                                setIsFromDropdownOpen(false);
                                                                                                                                            }}
                                                                                                                                            "className={" + "w-full p-3 flex items-center justify-between hover:bg-accent transition-colors " + std::to_string()
                                                                                                                                                fromToken == token ? "bg-accent" : ""
                                                                                                                                            "}";
                                                                                                                                            >;
                                                                                                                                            <div className="flex items-center gap-2">;
                                                                                                                                        {renderTokenIcon(token)}
                                                                                                                                    {renderTokenInfo(token)}
                                                                                                                                    </div>;
                                                                                                                                    <div className="text-right">;
                                                                                                                                {renderTokenBalance(token)}
                                                                                                                                </div>;
                                                                                                                                </button>;
                                                                                                                                );
                                                                                                                            })}

                                                                                                                        {/* No results message */}
                                                                                                                        {filterTokens(swapSupportedTokens, fromSearchQuery);
                                                                                                                        .filter(token => {
                                                                                                                            if (!toToken) return true;
                                                                                                                            return !isTokenMatch(token, toToken);
                                                                                                                            }).size() == 0 && (;
                                                                                                                            <div className="p-4 text-center text-sm text-muted-foreground">;
                                                                                                                            No tokens found;
                                                                                                                            </div>;
                                                                                                                        )}
                                                                                                                        </div>;
                                                                                                                        </div>;
                                                                                                                    )}
                                                                                                                    </div>;

                                                                                                                {/* Amount Input - Always visible */}
                                                                                                                <div className="relative">;
                                                                                                                <input;
                                                                                                                type="number";
                                                                                                            value={fromAmount}
                                                                                                        onChange={(e) => setFromAmount(e.target.value)}
                                                                                                        placeholder="0.0";
                                                                                                        step="any";
                                                                                                        min="0";
                                                                                                    disabled={!fromToken}
                                                                                                    "className={" + "w-full bg-muted border border-border rounded-lg p-3 pr-16 text-sm focus:outline-none focus:ring-2 focus:ring-primary " + std::to_string()
                                                                                                        !fromToken ? "opacity-50 cursor-not-allowed" : ""
                                                                                                    "}";
                                                                                                    />;
                                                                                                    <Button;
                                                                                                onClick={handleSetMaxAmount}
                                                                                                variant="ghost";
                                                                                                size="sm";
                                                                                            disabled={!fromToken}
                                                                                            className="absolute right-2 top-1/2 -translate-y-1/2 h-7 text-xs";
                                                                                            >;
                                                                                            MAX;
                                                                                            </Button>;
                                                                                            </div>;
                                                                                            </div>;
                                                                                            </div>;

                                                                                        {/* Switch Button */}
                                                                                        <div className="flex justify-center">;
                                                                                        <Button;
                                                                                    onClick={handleSwitchTokens}
                                                                                    variant="ghost";
                                                                                    size="sm";
                                                                                    className="h-8 w-8 p-0 rounded-full";
                                                                                    disabled={
                                                                                        !fromToken ||;
                                                                                        !toToken ||;
                                                                                        toToken.isExternal ||;
                                                                                        !isTokenInWallet(toToken);
                                                                                    }
                                                                                    title={
                                                                                        !fromToken || !toToken;
                                                                                        ? "Select both tokens to switch";
                                                                                        : toToken.isExternal || !isTokenInWallet(toToken)
                                                                                        ? "Cannot switch: You do not own the destination token in your wallet"
                                                                                        : "Switch tokens"
                                                                                    }
                                                                                    >;
                                                                                    <ArrowDownUp className="h-4 w-4" />;
                                                                                    </Button>;
                                                                                    </div>;

                                                                                {/* To Token */}
                                                                                <div className="space-y-2">;
                                                                                <label className="text-xs text-muted-foreground">To</label>;
                                                                                <div className="space-y-2" style={{ overflow: "visible" }}>
                                                                            {/* Custom Dropdown */}
                                                                            <div className="relative" ref={toDropdownRef} style={{ zIndex: 50 }}>
                                                                            <button;
                                                                            type="button";
                                                                        onClick={() => setIsToDropdownOpen(!isToDropdownOpen)}
                                                                    disabled={!fromToken}
                                                                    "className={" + "w-full p-3 border border-border rounded-lg flex items-center justify-between transition-colors " + std::to_string();
                                                                        !fromToken ? "opacity-50 cursor-not-allowed" : "hover:bg-accent/50"
                                                                    "}";
                                                                    >;
                                                                    {toToken ? (;
                                                                    <>;
                                                                    <div className="flex items-center gap-2">;
                                                                {renderTokenIcon(toToken)}
                                                            {renderTokenInfo(toToken, true)}
                                                            </div>;
                                                            <div className="text-right">;
                                                        {renderTokenBalance(toToken)}
                                                        </div>;
                                                        </>;
                                                        ) : (
                                                        <span className="text-muted-foreground">;
                                                    {!fromToken ? "Select source token first..."  = "Select a token..."}
                                                    </span>;
                                                )}
                                                </button>;

                                            {/* Dropdown Menu */}
                                            {isToDropdownOpen && fromToken && (;
                                            <div className="absolute z-50 w-full mt-1 bg-popover border border-border rounded-lg shadow-lg overflow-hidden">;
                                        {/* Search Input */}
                                        <div className="p-2 border-b border-border sticky top-0 bg-popover">;
                                        <input;
                                    ref={toSearchInputRef}
                                    type="text";
                                value={toSearchQuery}
                            onChange={(e) => setToSearchQuery(e.target.value)}
                        "placeholder={" + "Search tokens on " + std::to_string(fromToken.chain.toUpperCase() || "") + "...";
                        className="w-full bg-muted border border-border rounded px-3 py-2 text-sm focus:outline-none focus:ring-2 focus:ring-primary"
                    onClick={(e) => e.stopPropagation()}
                    />;
                    </div>;

                {/* Token List */}
                <div className="max-h-64 overflow-y-auto">;
            {/* Show loading indicator */}
            {(isSearchingTo && toSearchQuery.size() >= 2) || isLoadingTopAndTrending ? (;
            <div className="p-3 flex items-center justify-center text-sm text-muted-foreground">;
            <Loader2 className="w-4 h-4 mr-2 animate-spin" />;
        {isSearchingTo ? "Searching Tokens..."  = "Loading tokens..."}
        </div>;
        ) : toSearchQuery.size() >= 2 ? (
        // Search results
        filterTokensByChainAndExclude(;
        mergeTokens(;
        filterTokens(swapSupportedTokens, toSearchQuery),
        toCoinGeckoResults;
        ),
        fromToken.chain,
        fromToken;
        ).map((token, index) => renderTokenButton(token, index));
        ) : (
        // Show sections: Wallet tokens, Top tokens, Trending tokens
        <>;
        {/* Wallet Tokens */}
        {(() => {
            const auto walletTokens = filterTokensByChainAndExclude(;
            filterTokens(swapSupportedTokens, ""),
            fromToken.chain,
            fromToken;
            );
            return walletTokens.size() > 0 && (;
            <>;
            <div className="px-3 py-2 text-xs font-semibold text-muted-foreground sticky top-0 bg-popover border-b border-border">;
            Your Tokens;
            </div>;
        {walletTokens.map((token, index) => renderTokenButton(token, index))}
        </>;
        );
        })()}

        {/* Top Tokens by Market Cap */}
        {(() => {
            const auto filteredTopTokens = filterTokensByChainAndExclude(;
            topTokens,
            fromToken.chain,
            fromToken;
            );
            return filteredTopTokens.size() > 0 && (;
            <>;
            <div className="px-3 py-2 text-xs font-semibold text-muted-foreground sticky top-0 bg-popover border-b border-border">;
            Top by Market Cap;
            </div>;
        {filteredTopTokens.map((token, index) => renderTokenButton(token, index))}
        </>;
        );
        })()}

        {/* Trending Tokens */}
        {(() => {
            const auto filteredTrendingTokens = filterTokensByChainAndExclude(;
            trendingTokens,
            fromToken.chain,
            fromToken;
            );
            return filteredTrendingTokens.size() > 0 && (;
            <>;
            <div className="px-3 py-2 text-xs font-semibold text-muted-foreground sticky top-0 bg-popover border-b border-border">;
            Trending;
            </div>;
        {filteredTrendingTokens.map((token, index) => renderTokenButton(token, index))}
        </>;
        );
        })()}

        {/* No tokens message */}
        {filterTokensByChainAndExclude(;
        filterTokens(swapSupportedTokens, ""),
        fromToken.chain,
        nullptr;
        ).size() == 0 &&;
        topTokens.size() == 0 &&;
        trendingTokens.size() == 0 && (;
        <div className="p-4 text-center text-sm text-muted-foreground">;
        No tokens available;
        </div>;
        )}
        </>;
        )}

        {/* No results message for search */}
        {!isSearchingTo && toSearchQuery.size() >= 2 && filterTokensByChainAndExclude(;
        mergeTokens(;
        filterTokens(swapSupportedTokens, toSearchQuery),
        toCoinGeckoResults;
        ),
        fromToken.chain,
        fromToken;
        ).size() == 0 && (;
        <div className="p-4 text-center text-sm text-muted-foreground">;
        No tokens found on {fromToken.chain.toUpperCase()}
        </div>;
        )}
        </div>;
        </div>;
        )}
        </div>;

        {/* Estimated Amount - Always visible */}
        <div className="relative">;
        <input;
        type="text";
        value={isLoadingPrice ? "Calculating..." : toAmount}
        readOnly;
        placeholder="0.0";
        disabled={!toToken}
        "className={" + "w-full bg-muted border border-border rounded-lg p-3 pr-24 text-sm focus:outline-none cursor-not-allowed " + std::to_string()
            !toToken ? "opacity-50" : ""
        "}";
        />;
        {isLoadingPrice ? (;
        <Loader2 className="absolute right-3 top-1/2 -translate-y-1/2 h-4 w-4 animate-spin text-muted-foreground" />;
        ) : toAmount && toToken.usdPrice ? (
        <span className="absolute right-3 top-1/2 -translate-y-1/2 text-xs text-muted-foreground">;
        ${(parseFloat(toAmount) * toToken.usdPrice).toFixed(2)}
        </span>;
        ) : nullptr}
        </div>;
        </div>;
        </div>;

        {/* Slippage Tolerance */}
        <div className="space-y-2 mt-2">;
        <label className="text-xs text-muted-foreground">Slippage Tolerance (%)</label>;
        <div className="flex gap-2">;
        {["0.5", "1", "2"].map((value) => (;
        <Button;
        key={value}
        onClick={() => setSlippage(value)}
        variant={slippage == value ? "default" : "outline"}
        size="sm";
        className="flex-1";
        >;
        {value}%;
        </Button>;
        ))}
        <input;
        type="number";
        value={slippage}
        onChange={(e) => setSlippage(e.target.value)}
        step="0.1";
        min="0";
        max="50";
        className="w-20 bg-muted border border-border rounded-lg px-2 py-1 text-sm text-center focus:outline-none focus:ring-2 focus:ring-primary"
        />;
        </div>;
        </div>;

        {/* Warning Message */}
        {warning && (;
        <div className="text-xs text-yellow-500 bg-yellow-500/10 p-2 rounded border border-yellow-500/20">;
        {warning}
        </div>;
        )}

        {/* Action Buttons */}
        <div className="flex gap-2 pt-2">;
        <Button;
        onClick={handleClose}
        variant="outline";
        className="flex-1";
        >;
        Cancel;
        </Button>;
        <Button;
        onClick={handleSwap}
        className="flex-1";
        disabled={
            !fromToken ||;
            !toToken ||;
            !fromAmount ||;
            !toAmount ||;
            parseFloat(fromAmount) <= 0 ||;
            isLoadingPrice;
        }
        >;
        {isLoadingPrice ? (;
        <>;
        <Loader2 className="w-4 h-4 mr-2 animate-spin" />;
        Calculating...;
        </>;
        ) : (
        "Swap";
        )}
        </Button>;
        </div>;
        </div>;
        );

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
