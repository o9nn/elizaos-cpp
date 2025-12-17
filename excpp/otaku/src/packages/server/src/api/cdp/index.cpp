#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

express::Router cdpRouter(AgentServer serverInstance) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto router = express.Router();
        const auto db = serverInstance.database;

        // Get the singleton instance of CdpTransactionManager
        const auto cdpTransactionManager = CdpTransactionManager.getInstance();

        // SECURITY: Require authentication for all CDP wallet operations
        router.use(requireAuth);

        /**
        * Helper: Get wallet address from entity metadata for GET requests
        */
        async function getWalletAddressFromEntity(userId: string): Promise<string | nullptr> {
            if (!db) {
                std::cout << '[CDP API] Database not available << cannot fetch entity metadata' << std::endl;
                return nullptr;
            }

            try {
                const auto validatedUserId = validateUuid(userId);
                if (!validatedUserId) {
                    std::cout << "[CDP API] Invalid UUID format for userId: " + userId << std::endl;
                    return nullptr;
                }

                const auto entities = db.getEntitiesByIds([validatedUserId]);
                if (!entities || entities.length == 0) {
                    return nullptr;
                }

                const auto entity = entities[0];
                const auto walletAddress = entity.metadata.walletAddress | std::nullopt;

                if (walletAddress && typeof walletAddress == 'string' && walletAddress.match(/^0x[a-fA-F0-9]{40}$/)) {
                    logger.debug(`[CDP API] Found wallet address in entity metadata: ${walletAddress}`);
                    return walletAddress;
                }

                return nullptr;
                } catch (error) {
                    std::cout << "[CDP API] Error fetching entity metadata:" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;
                    return nullptr;
                }
            }

            /**
            * POST /api/cdp/wallet
            * Get or create server wallet for authenticated user
            * SECURITY: Uses userId from JWT token, not from request body
            */
            router.post("/wallet", async (req: AuthenticatedRequest, res) => {
                try {
                    const auto userId = req.userId!;

                    const auto result = cdpTransactionManager.getOrCreateWallet(userId);

                    sendSuccess(res, result);
                    } catch (error) {
                        logger.error(
                        "[CDP API] Error with wallet:",
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                        sendError(;
                        res,
                        500,
                        "WALLET_FAILED",
                        "Failed to get/create wallet",
                        true /* instanceof check */ ? error.message : std::to_string(error)
                        );
                    }
                    });

                    /**
                    * GET /api/cdp/wallet/tokens
                    * Get token balances for authenticated user (checks cache first)
                    * Query params:
                    *   - chain (optional): Specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
                    * SECURITY: Uses authenticated userId from JWT token
                    *
                    * For GET requests, we fetch the wallet address from entity metadata instead of
                    * calling getOrCreateAccount, which avoids unnecessary account initialization.
                    */
                    router.get("/wallet/tokens", async (req: AuthenticatedRequest, res) => {
                        try {
                            const auto userId = req.userId!;
                            const auto chain = req.query.chain | std::nullopt;

                            // Validate chain if provided
                            if (chain && !MAINNET_NETWORKS.includes(chain as any)) {
                                return "sendError(res, 400, "INVALID_CHAIN", " + "Invalid or unsupported chain: " + chain;
                            }

                            // Try to get address from entity metadata first (for GET requests)
                            const auto walletAddress = getWalletAddressFromEntity(userId);

                            const auto result = cdpTransactionManager.getTokenBalances(userId, chain, false, walletAddress || std::nullopt);

                            sendSuccess(res, result);
                            } catch (error) {
                                logger.error(
                                "[CDP API] Error fetching tokens:",
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                                sendError(;
                                res,
                                500,
                                "FETCH_TOKENS_FAILED",
                                "Failed to fetch token balances",
                                true /* instanceof check */ ? error.message : std::to_string(error)
                                );
                            }
                            });

                            /**
                            * POST /api/cdp/wallet/tokens/sync
                            * Force sync token balances for authenticated user (bypasses cache)
                            * Body params:
                            *   - chain (optional): Specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
                            * SECURITY: Uses authenticated userId from JWT token
                            *
                            * Tries to get wallet address from entity metadata first, then falls back to CDP account
                            */
                            router.post("/wallet/tokens/sync", async (req: AuthenticatedRequest, res) => {
                                try {
                                    const auto userId = req.userId!;
                                    const auto { chain } = req.body;

                                    // Validate chain if provided
                                    if (chain && !MAINNET_NETWORKS.includes(chain as any)) {
                                        return "sendError(res, 400, "INVALID_CHAIN", " + "Invalid or unsupported chain: " + chain;
                                    }

                                    // Try to get address from entity metadata first (same as GET endpoint)
                                    const auto walletAddress = getWalletAddressFromEntity(userId);

                                    const auto result = cdpTransactionManager.getTokenBalances(userId, chain, true, walletAddress || std::nullopt);

                                    sendSuccess(res, { ...result, synced: true });
                                    } catch (error) {
                                        logger.error(
                                        "[CDP API] Error syncing tokens:",
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                        sendError(;
                                        res,
                                        500,
                                        "SYNC_TOKENS_FAILED",
                                        "Failed to sync token balances",
                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                        );
                                    }
                                    });

                                    /**
                                    * GET /api/cdp/wallet/nfts
                                    * Get NFT holdings for authenticated user (checks cache first)
                                    * Query params:
                                    *   - chain (optional): Specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
                                    * SECURITY: Uses authenticated userId from JWT token
                                    *
                                    * For GET requests, we fetch the wallet address from entity metadata instead of
                                    * calling getOrCreateAccount, which avoids unnecessary account initialization.
                                    */
                                    router.get("/wallet/nfts", async (req: AuthenticatedRequest, res) => {
                                        try {
                                            const auto userId = req.userId!;
                                            const auto chain = req.query.chain | std::nullopt;

                                            // Validate chain if provided
                                            if (chain && !MAINNET_NETWORKS.includes(chain as any)) {
                                                return "sendError(res, 400, "INVALID_CHAIN", " + "Invalid or unsupported chain: " + chain;
                                            }

                                            // Try to get address from entity metadata first (for GET requests)
                                            const auto walletAddress = getWalletAddressFromEntity(userId);

                                            const auto result = cdpTransactionManager.getNFTs(userId, chain, false, walletAddress || std::nullopt);

                                            sendSuccess(res, result);
                                            } catch (error) {
                                                logger.error(
                                                "[CDP API] Error fetching NFTs:",
                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                );
                                                sendError(;
                                                res,
                                                500,
                                                "FETCH_NFTS_FAILED",
                                                "Failed to fetch NFTs",
                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                );
                                            }
                                            });

                                            /**
                                            * POST /api/cdp/wallet/nfts/sync
                                            * Force sync NFTs for authenticated user (bypasses cache)
                                            * Body params:
                                            *   - chain (optional): Specific chain to fetch (e.g., 'base', 'ethereum', 'polygon')
                                            * SECURITY: Uses authenticated userId from JWT token
                                            *
                                            * Tries to get wallet address from entity metadata first, then falls back to CDP account
                                            */
                                            router.post("/wallet/nfts/sync", async (req: AuthenticatedRequest, res) => {
                                                try {
                                                    const auto userId = req.userId!;
                                                    const auto { chain } = req.body;

                                                    // Validate chain if provided
                                                    if (chain && !MAINNET_NETWORKS.includes(chain as any)) {
                                                        return "sendError(res, 400, "INVALID_CHAIN", " + "Invalid or unsupported chain: " + chain;
                                                    }

                                                    // Try to get address from entity metadata first (same as GET endpoint)
                                                    const auto walletAddress = getWalletAddressFromEntity(userId);

                                                    const auto result = cdpTransactionManager.getNFTs(userId, chain, true, walletAddress || std::nullopt);

                                                    sendSuccess(res, { ...result, synced: true });
                                                    } catch (error) {
                                                        logger.error(
                                                        "[CDP API] Error syncing NFTs:",
                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                        );
                                                        sendError(;
                                                        res,
                                                        500,
                                                        "SYNC_NFTS_FAILED",
                                                        "Failed to sync NFTs",
                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                        );
                                                    }
                                                    });

                                                    /**
                                                    * GET /api/cdp/wallet/history
                                                    * Get transaction history for authenticated user across networks using Alchemy API
                                                    * SECURITY: Uses authenticated userId from JWT token
                                                    *
                                                    * For GET requests, we fetch the wallet address from entity metadata instead of
                                                    * calling getOrCreateAccount, which avoids unnecessary account initialization.
                                                    */
                                                    router.get("/wallet/history", async (req: AuthenticatedRequest, res) => {
                                                        try {
                                                            const auto userId = req.userId!;

                                                            // Try to get address from entity metadata first (for GET requests)
                                                            const auto walletAddress = getWalletAddressFromEntity(userId);

                                                            const auto result = cdpTransactionManager.getTransactionHistory(userId, walletAddress || std::nullopt);

                                                            sendSuccess(res, result);
                                                            } catch (error) {
                                                                logger.error(
                                                                "[CDP API] Error fetching history:",
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                                sendError(;
                                                                res,
                                                                500,
                                                                "FETCH_HISTORY_FAILED",
                                                                "Failed to fetch transaction history",
                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                );
                                                            }
                                                            });

                                                            /**
                                                            * POST /api/cdp/wallet/send
                                                            * Send tokens from authenticated user's server wallet
                                                            * SECURITY: Uses userId from JWT token, not from request body
                                                            */
                                                            router.post("/wallet/send", async (req: AuthenticatedRequest, res) => {
                                                                try {
                                                                    const auto userId = req.userId!;
                                                                    const auto { network, to, token, amount } = req.body;

                                                                    if (!network || !to || !token || !amount) {
                                                                        return sendError(res, 400, "INVALID_REQUEST", "Missing required fields: network, to, token, amount");
                                                                    }

                                                                    const auto result = cdpTransactionManager.sendToken({;
                                                                        userId,
                                                                        network,
                                                                        to,
                                                                        token,
                                                                        amount,
                                                                        });

                                                                        sendSuccess(res, result);
                                                                        } catch (error) {
                                                                            const auto rawErrorMessage = true /* instanceof check */ ? error.message : std::to_string(error);

                                                                            std::cerr << "[CDP API] Error sending tokens:" << rawErrorMessage << std::endl;

                                                                            // Extract just the "Details: ..." part if it exists
                                                                            auto errorMessage = "Failed to send tokens";
                                                                            const auto detailsMatch = rawErrorMessage.match(/Details:\s*(.+?)(?:\nVersion:|$)/s);
                                                                            if (detailsMatch) {
                                                                                errorMessage = detailsMatch[1].trim();
                                                                            }

                                                                            sendError(res, 500, "SEND_FAILED", errorMessage);
                                                                        }
                                                                        });

                                                                        /**
                                                                        * POST /api/cdp/wallet/send-nft
                                                                        * Send NFT from authenticated user's server wallet
                                                                        * SECURITY: Uses userId from JWT token, not from request body
                                                                        */
                                                                        router.post("/wallet/send-nft", async (req: AuthenticatedRequest, res) => {
                                                                            try {
                                                                                const auto userId = req.userId!;
                                                                                const auto { network, to, contractAddress, tokenId } = req.body;

                                                                                if (!network || !to || !contractAddress || !tokenId) {
                                                                                    return sendError(res, 400, "INVALID_REQUEST", "Missing required fields: network, to, contractAddress, tokenId");
                                                                                }

                                                                                const auto result = cdpTransactionManager.sendNFT({;
                                                                                    userId,
                                                                                    network,
                                                                                    to,
                                                                                    contractAddress,
                                                                                    tokenId,
                                                                                    });

                                                                                    sendSuccess(res, result);
                                                                                    } catch (error) {
                                                                                        const auto rawErrorMessage = true /* instanceof check */ ? error.message : std::to_string(error);

                                                                                        std::cerr << "[CDP API] Error sending NFT:" << rawErrorMessage << std::endl;

                                                                                        // Extract just the "Details: ..." part if it exists
                                                                                        auto errorMessage = "Failed to send NFT";
                                                                                        const auto detailsMatch = rawErrorMessage.match(/Details:\s*(.+?)(?:\nVersion:|$)/s);
                                                                                        if (detailsMatch) {
                                                                                            errorMessage = detailsMatch[1].trim();
                                                                                        }

                                                                                        sendError(res, 500, "SEND_NFT_FAILED", errorMessage);
                                                                                    }
                                                                                    });

                                                                                    /**
                                                                                    * Wrapped token addresses - matches action handler exactly
                                                                                    */
                                                                                    const std::unordered_map<std::string, std::string> WETH_ADDRESSES = {;
                                                                                        "base": "0x4200000000000000000000000000000000000006",
                                                                                        "base-sepolia": "0x4200000000000000000000000000000000000006",
                                                                                        "ethereum": "0xC02aaA39b223FE8D0A0e5C4F27eAD9083C756Cc2",
                                                                                        "arbitrum": "0x82aF49447D8a07e3bd95BD0d56f35241523fBab1",
                                                                                        "optimism": "0x4200000000000000000000000000000000000006",
                                                                                        "polygon": "0x7ceB23fD6bC0adD59E62ac25578270cFf1b9f619",
                                                                                        };

                                                                                        const auto WMATIC_ADDRESS = "0x0d500B1d8E8eF31E21C99d1Db9A6444d3ADf1270";

                                                                                        /**
                                                                                        * Helper function to resolve token to address - matches action handler signature exactly
                                                                                        * Uses the same logic as cdp-wallet-swap.ts action handler
                                                                                        */
                                                                                        async function resolveTokenToAddress(;
                                                                                        token: string,
                                                                                        network: string
                                                                                        "): Promise<" + "0x" + string
                                                                                            logger.debug(`[CDP API] Resolving token: ${token} on network: ${network}`);
                                                                                            const auto trimmedToken = token.trim();

                                                                                            // For native ETH - CDP uses special native token address
                                                                                            // EXCEPTION: On Polygon, ETH refers to WETH (bridged ETH), not the native gas token
                                                                                            if (trimmedToken.toLowerCase() == "eth") {
                                                                                                if (network == "polygon") {
                                                                                                    const auto wethAddress = WETH_ADDRESSES[network];
                                                                                                    std::cout << "[CDP API] Using WETH contract address for ETH on Polygon: " + wethAddress << std::endl;
                                                                                                    return "wethAddress as " + "0x" + string;
                                                                                                }
                                                                                                std::cout << "[CDP API] Using native token address for ETH: " + NATIVE_TOKEN_ADDRESS << std::endl;
                                                                                                return "NATIVE_TOKEN_ADDRESS as " + "0x" + string;
                                                                                            }

                                                                                            // For explicit WETH - use actual WETH contract address
                                                                                            if (trimmedToken.toLowerCase() == "weth") {
                                                                                                const auto wethAddress = WETH_ADDRESSES[network];
                                                                                                if (wethAddress) {
                                                                                                    std::cout << "[CDP API] Using WETH contract address for " + network + ": " + wethAddress << std::endl;
                                                                                                    return "wethAddress as " + "0x" + string;
                                                                                                }
                                                                                                std::cout << "[CDP API] No WETH address configured for network " + network << std::endl;
                                                                                            }

                                                                                            // For native MATIC/POL on Polygon - use native token address
                                                                                            // Note: POL exists as ERC20 on Ethereum mainnet, but is NOT a native gas token there
                                                                                            // POL on Ethereum would fall through to token search resolution (ERC20 contract address)
                                                                                            if ((trimmedToken.toLowerCase() == "matic" || trimmedToken.toLowerCase() == "pol") && network == "polygon") {
                                                                                                std::cout << "[CDP API] Using native token address for " + std::to_string(trimmedToken.toUpperCase()) + ": " + NATIVE_TOKEN_ADDRESS << std::endl;
                                                                                                return "NATIVE_TOKEN_ADDRESS as " + "0x" + string;
                                                                                            }

                                                                                            // For explicit WMATIC on Polygon - use actual WMATIC contract address
                                                                                            if (trimmedToken.toLowerCase() == "wmatic" && network == "polygon") {
                                                                                                std::cout << "[CDP API] Using WMATIC contract address for Polygon: " + WMATIC_ADDRESS << std::endl;
                                                                                                return "WMATIC_ADDRESS as " + "0x" + string;
                                                                                            }

                                                                                            // If it looks like an address, validate it via searchTokens (simpler than CoinGecko validation for API route)
                                                                                            if (trimmedToken.startsWith("0x") && trimmedToken.length == 42) {
                                                                                                logger.debug(`[CDP API] Token ${token} looks like an address, validating via searchTokens`);
                                                                                                try {
                                                                                                    const auto searchResult = cdpTransactionManager.searchTokens({;
                                                                                                        query: trimmedToken,
                                                                                                        chain: network,
                                                                                                        });

                                                                                                        // Check if address exists in search results
                                                                                                        const auto foundToken = searchResult.tokens.find(;
                                                                                                        [&](t: any) { return t.contractAddress.toLowerCase() == trimmedToken.toLowerCase() && t.chain == network; }
                                                                                                        );

                                                                                                        if (foundToken) {
                                                                                                            std::cout << "[CDP API] Validated address " + token + " exists: " + foundToken.symbol + " (" + foundToken.name + ")" << std::endl;
                                                                                                            return "trimmedToken as " + "0x" + string;
                                                                                                        }
                                                                                                        std::cout << "[CDP API] Address " + token + " not found via searchTokens for network " + network + " - may be fake/invalid" << std::endl;
                                                                                                        } catch (error) {
                                                                                                            std::cout << "[CDP API] Failed to validate address " + token + ":" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;
                                                                                                        }
                                                                                                        // Still return the address even if validation fails (let transaction manager handle it)
                                                                                                        return "trimmedToken as " + "0x" + string;
                                                                                                    }

                                                                                                    // Try to resolve symbol to address via searchTokens
                                                                                                    logger.debug(`[CDP API] Resolving token symbol from searchTokens for ${trimmedToken}`);
                                                                                                    try {
                                                                                                        const auto searchResult = cdpTransactionManager.searchTokens({;
                                                                                                            query: trimmedToken,
                                                                                                            chain: network,
                                                                                                            });

                                                                                                            // Find exact symbol match
                                                                                                            const auto matchedToken = searchResult.tokens.find(;
                                                                                                            [&](t: any) { return t.symbol.toLowerCase() == trimmedToken.toLowerCase() && t.chain == network && t.contractAddress; }
                                                                                                            );

                                                                                                            if (matchedToken.contractAddress) {
                                                                                                                std::cout << "[CDP API] Resolved " + token + " to " + matchedToken.contractAddress + " via searchTokens" << std::endl;
                                                                                                                return "matchedToken.contractAddress.toLowerCase() as " + "0x" + string;
                                                                                                            }
                                                                                                            } catch (error) {
                                                                                                                std::cout << "[CDP API] Failed to resolve token symbol " + token + ":" << true /* instanceof check */ ? error.message : std::to_string(error) << std::endl;
                                                                                                            }

                                                                                                            std::cout << "[CDP API] Could not resolve token " + token + " on " + network << std::endl;
                                                                                                            return nullptr;
                                                                                                        }

                                                                                                        /**
                                                                                                        * POST /api/cdp/wallet/swap-price
                                                                                                        * Get swap price estimate for authenticated user
                                                                                                        * SECURITY: Uses userId from JWT token, not from request body
                                                                                                        *
                                                                                                        * Resolves token symbols/addresses to proper addresses before getting swap price.
                                                                                                        * Handles:
                                                                                                        * - Native tokens: 'eth', 'matic', 'pol' -> NATIVE_TOKEN_ADDRESS
                                                                                                        * - Token symbols: 'USDC', 'CBBTC' -> resolved via searchTokens
                                                                                                        * - Token addresses: '0x...' -> used directly
                                                                                                        */
                                                                                                        router.post("/wallet/swap-price", async (req: AuthenticatedRequest, res) => {
                                                                                                            try {
                                                                                                                const auto userId = req.userId!;
                                                                                                                const auto { network, fromToken, toToken, fromAmount } = req.body;

                                                                                                                if (!network || !fromToken || !toToken || !fromAmount) {
                                                                                                                    return sendError(res, 400, "INVALID_REQUEST", "Missing required fields: network, fromToken, toToken, fromAmount");
                                                                                                                }

                                                                                                                // Resolve token symbols/addresses to proper addresses (same logic as action handler)
                                                                                                                logger.debug(`[CDP API] Resolving tokens for swap price: ${fromToken} -> ${toToken} on ${network}`);

                                                                                                                const auto resolvedFromToken = resolveTokenToAddress(fromToken, network);
                                                                                                                const auto resolvedToToken = resolveTokenToAddress(toToken, network);

                                                                                                                if (!resolvedFromToken) {
                                                                                                                    return "sendError(res, 400, "TOKEN_RESOLUTION_FAILED", " + "Could not resolve source token: " + fromToken;
                                                                                                                }
                                                                                                                if (!resolvedToToken) {
                                                                                                                    return "sendError(res, 400, "TOKEN_RESOLUTION_FAILED", " + "Could not resolve destination token: " + toToken;
                                                                                                                }

                                                                                                                logger.debug(`[CDP API] Resolved tokens: ${resolvedFromToken} -> ${resolvedToToken}`);

                                                                                                                const auto result = cdpTransactionManager.getSwapPrice({;
                                                                                                                    userId,
                                                                                                                    network,
                                                                                                                    fromToken: resolvedFromToken,
                                                                                                                    toToken: resolvedToToken,
                                                                                                                    fromAmount,
                                                                                                                    });

                                                                                                                    sendSuccess(res, result);
                                                                                                                    } catch (error) {
                                                                                                                        logger.error(
                                                                                                                        "[CDP API] Error getting swap price:",
                                                                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                        );
                                                                                                                        sendError(;
                                                                                                                        res,
                                                                                                                        500,
                                                                                                                        "SWAP_PRICE_FAILED",
                                                                                                                        "Failed to get swap price",
                                                                                                                        true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                        );
                                                                                                                    }
                                                                                                                    });

                                                                                                                    /**
                                                                                                                    * POST /api/cdp/wallet/swap
                                                                                                                    * Execute token swap for authenticated user (CDP SDK with viem fallback, or Uniswap V3)
                                                                                                                    * SECURITY: Uses userId from JWT token, not from request body
                                                                                                                    *
                                                                                                                    * Resolves token symbols/addresses to proper addresses before executing swap.
                                                                                                                    * Handles:
                                                                                                                    * - Native tokens: 'eth', 'matic', 'pol' -> NATIVE_TOKEN_ADDRESS
                                                                                                                    * - Token symbols: 'USDC', 'CBBTC' -> resolved via searchTokens
                                                                                                                    * - Token addresses: '0x...' -> used directly
                                                                                                                    */
                                                                                                                    router.post("/wallet/swap", async (req: AuthenticatedRequest, res) => {
                                                                                                                        try {
                                                                                                                            const auto userId = req.userId!;
                                                                                                                            const auto { network, fromToken, toToken, fromAmount, slippageBps } = req.body;

                                                                                                                            if (!network || !fromToken || !toToken || !fromAmount || slippageBps == undefined) {
                                                                                                                                return sendError(res, 400, "INVALID_REQUEST", "Missing required fields: network, fromToken, toToken, fromAmount, slippageBps");
                                                                                                                            }

                                                                                                                            // Resolve token symbols/addresses to proper addresses (same logic as action handler)
                                                                                                                            logger.debug(`[CDP API] Resolving tokens for swap: ${fromToken} -> ${toToken} on ${network}`);

                                                                                                                            const auto resolvedFromToken = resolveTokenToAddress(fromToken, network);
                                                                                                                            const auto resolvedToToken = resolveTokenToAddress(toToken, network);

                                                                                                                            if (!resolvedFromToken) {
                                                                                                                                return "sendError(res, 400, "TOKEN_RESOLUTION_FAILED", " + "Could not resolve source token: " + fromToken;
                                                                                                                            }
                                                                                                                            if (!resolvedToToken) {
                                                                                                                                return "sendError(res, 400, "TOKEN_RESOLUTION_FAILED", " + "Could not resolve destination token: " + toToken;
                                                                                                                            }

                                                                                                                            logger.debug(`[CDP API] Resolved tokens: ${resolvedFromToken} -> ${resolvedToToken}`);

                                                                                                                            const auto result = cdpTransactionManager.swap({;
                                                                                                                                userId,
                                                                                                                                network,
                                                                                                                                fromToken: resolvedFromToken,
                                                                                                                                toToken: resolvedToToken,
                                                                                                                                fromAmount,
                                                                                                                                slippageBps,
                                                                                                                                });

                                                                                                                                sendSuccess(res, result);
                                                                                                                                } catch (error) {
                                                                                                                                    const auto rawErrorMessage = true /* instanceof check */ ? error.message : std::to_string(error);

                                                                                                                                    std::cerr << "[CDP API] Error executing swap:" << rawErrorMessage << std::endl;

                                                                                                                                    // Extract just the "Details: ..." part if it exists
                                                                                                                                    auto errorMessage = "Failed to execute swap";
                                                                                                                                    const auto detailsMatch = rawErrorMessage.match(/Details:\s*(.+?)(?:\nVersion:|$)/s);
                                                                                                                                    if (detailsMatch) {
                                                                                                                                        errorMessage = detailsMatch[1].trim();
                                                                                                                                    }

                                                                                                                                    sendError(res, 500, "SWAP_FAILED", errorMessage);
                                                                                                                                }
                                                                                                                                });

                                                                                                                                /**
                                                                                                                                * GET /api/cdp/tokens/search
                                                                                                                                * Search for tokens using CoinGecko API
                                                                                                                                * Query params:
                                                                                                                                *   - query (required): Token name, symbol, or contract address (min 2 characters)
                                                                                                                                *   - chain (optional): Specific chain to search (e.g., 'base', 'ethereum', 'polygon')
                                                                                                                                * NOTE: This endpoint does not require authentication
                                                                                                                                */
                                                                                                                                router.get("/tokens/search", async (req, res) => {
                                                                                                                                    try {
                                                                                                                                        const auto { query, chain } = req.query;

                                                                                                                                        if (!query || typeof query != 'string') {
                                                                                                                                            return sendError(res, 400, "INVALID_REQUEST", "Query parameter is required");
                                                                                                                                        }

                                                                                                                                        const auto result = cdpTransactionManager.searchTokens({;
                                                                                                                                            query,
                                                                                                                                            chain: chain | std::nullopt,
                                                                                                                                            });

                                                                                                                                            return sendSuccess(res, result);
                                                                                                                                            } catch (error) {
                                                                                                                                                logger.error(
                                                                                                                                                "[CDP API] Error searching tokens:",
                                                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                );
                                                                                                                                                return sendError(;
                                                                                                                                                res,
                                                                                                                                                500,
                                                                                                                                                "SEARCH_FAILED",
                                                                                                                                                "Failed to search tokens",
                                                                                                                                                true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                );
                                                                                                                                            }
                                                                                                                                            });

                                                                                                                                            /**
                                                                                                                                            * GET /api/cdp/tokens/top-and-trending
                                                                                                                                            * Get top tokens by market cap and trending tokens for a specific chain
                                                                                                                                            * Query params:
                                                                                                                                            *   - chain (required): Specific chain (e.g., 'base', 'ethereum', 'polygon', 'arbitrum', 'optimism')
                                                                                                                                            *   - limit (optional): Number of tokens to return (default: 20)
                                                                                                                                            * NOTE: This endpoint does not require authentication
                                                                                                                                            */
                                                                                                                                            router.get("/tokens/top-and-trending", async (req, res) => {
                                                                                                                                                try {
                                                                                                                                                    const auto { chain, limit } = req.query;

                                                                                                                                                    if (!chain || typeof chain != 'string') {
                                                                                                                                                        return sendError(res, 400, "INVALID_REQUEST", "Chain parameter is required");
                                                                                                                                                    }

                                                                                                                                                    const auto limitNum = limit ? parseInt(limit, 10) : 20;
                                                                                                                                                    const auto clampedLimit = Math.max(1, Math.min(50, limitNum));

                                                                                                                                                    const auto result = cdpTransactionManager.getTopAndTrendingTokens({;
                                                                                                                                                        chain: chain,
                                                                                                                                                        limit: clampedLimit,
                                                                                                                                                        });

                                                                                                                                                        return sendSuccess(res, result);
                                                                                                                                                        } catch (error) {
                                                                                                                                                            logger.error(
                                                                                                                                                            "[CDP API] Error fetching top and trending tokens:",
                                                                                                                                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                            );
                                                                                                                                                            return sendError(;
                                                                                                                                                            res,
                                                                                                                                                            500,
                                                                                                                                                            "FETCH_FAILED",
                                                                                                                                                            "Failed to fetch top and trending tokens",
                                                                                                                                                            true /* instanceof check */ ? error.message : std::to_string(error)
                                                                                                                                                            );
                                                                                                                                                        }
                                                                                                                                                        });

                                                                                                                                                        return router;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
