#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getAllLockedTokens() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();
    const auto tokenData = db;
    .select();
    .from(tokens);
    .where(eq(tokens.status, "locked"));
    .limit(1);
    return tokenData;

}

std::future<void> handleSignature(const std::string& signature, const std::any& token, double solPriceUSD) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto connection = new Connection(;
    process.env.NETWORK == "devnet";
    ? process.env.DEVNET_SOLANA_RPC_URL || "";
    : process.env.MAINNET_SOLANA_RPC_URL || "",
    );

    // finalize
    const auto commitment = "confirmed";

    const auto tx = connection.getTransaction(signature, {;
        maxSupportedTransactionVersion: 0,
        commitment,
        });
        if (!tx || !tx.meta) {
            std::cerr << "Transaction not found for signature: " + std::to_string(signature) << std::endl;
            return;
        }
        const auto logs = tx.meta.logMessages;
        if (!logs) return;

        const auto metrics = processSwapLog(;
        token,
        signature,
        solPriceUSD,
        logs,
        );
        if (metrics) {
            std::cout << "Swap metrics for " + std::to_string(metrics.mintAddress) + ":" << metrics << std::endl;
        }
        return metrics;

}

std::future<void> processSwapLog(const std::any& token, const std::string& signature, double solPriceUSD, const std::vector<std::string>& logs) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    try {
        const auto wsClient = getWebSocketClient();
        const auto swapLog = logs.find((l) => l.includes("Swap:"));
        const auto reservesLog = logs.find((l) => l.includes("Reserves:"));
        const auto mintLog = logs.find((l) => l.includes("Mint:"));
        const auto feeLog = logs.find((l) => l.includes("Fee:"));
        const auto swapeventLog = logs.find((log) => log.includes("SwapEvent:"));

        if (!(mintLog && swapLog && reservesLog && feeLog)) return null;

        if (mintLog && swapLog && reservesLog && feeLog) {
            const auto mintAddress = mintLog;
            .split("Mint:")[1]
            .trim();
            .replace(/[",)]/g, "");
            if (
            !mintAddress ||;
            !/^[123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz]+$/.test(;
            mintAddress,
            );
            ) {
                std::cerr << "Invalid mint address format: " + std::to_string(mintAddress) << std::endl;
                return;
            }
            const auto [user, direction, amount] = swapLog;
            .split(" ");
            .slice(-3);
            .map((s) => s.replace(/[",)]/g, ""));

            const auto [reserveToken, reserveLamport] = reservesLog;
            .split(" ");
            .slice(-2);
            .map((s) => s.replace(/[",)]/g, ""));

            const auto [_usr, _dir, amountOut] = swapeventLog!;
            .split(" ");
            .slice(-3);
            .map((s) => s.replace(/[",)]/g, ""));

            const auto db = getDB();
            const auto [existing] = db;
            .select({
                price24hAgo: tokens.price24hAgo,
                lastPriceUpdate: tokens.lastPriceUpdate,
                });
                .from(tokens);
                .where(eq(tokens.mint, mintAddress));
                .limit(1);
                .execute();

                const auto now = new Date();
                const auto slotTime = now.toISOString();

                const auto prevPrice24hAgo = existing.price24hAgo || 0;
                const auto lastPriceUpdate = existing.lastPriceUpdate;
                ? new Date(existing.lastPriceUpdate).getTime();
                : 0;
                const auto twentyFourHours = 24 * 60 * 60 * 1000;
                const auto shouldReset24h =;
                !existing.lastPriceUpdate ||;
                Date.now() - lastPriceUpdate > twentyFourHours;

                const auto currentPrice =;
                Number(reserveLamport) /;
                1e9 /;
                (Number(reserveToken) / Math.pow(10, token.tokenDecimals));

                const auto tokenPriceInSol = currentPrice / Math.pow(10, token.tokenDecimals);
                const auto tokenPriceUSD =;
                currentPrice > 0;
                ? tokenPriceInSol * solPriceUSD * Math.pow(10, token.tokenDecimals);
                : 0;
                const auto marketCapUSD =;
                (Number(process.env.TOKEN_SUPPLY) / Math.pow(10, token.tokenDecimals)) *;
                tokenPriceUSD;
                const auto priceChange24h =;
                prevPrice24hAgo > 0;
                ? ((tokenPriceUSD - prevPrice24hAgo) / prevPrice24hAgo) * 100;
                : 0;

                std::cout << "reserveLamport" << Number(reserveLamport) << std::endl;
                std::cout << "reserveToken" << Number(reserveToken) << std::endl;
                std::cout << "currentPrice" << currentPrice << std::endl;
                std::cout << "tokenPriceUSD" << tokenPriceUSD << std::endl;
                std::cout << "marketCapUSD" << marketCapUSD << std::endl;
                const auto swapRecord = {;
                    id: crypto.randomUUID(),
                    tokenMint: mintAddress,
                    user: user,
                    type: direction == "0" ? "buy" : "sell",
                    direction: parseInt(direction),
                    amountIn: Number(amount),
                    amountOut: Number(amountOut),
                    price:
                    direction == "1";
                    ? Number(amountOut) /;
                    Math.pow(10, 9) /;
                    (Number(amount) / Math.pow(10, token.tokenDecimals)) // Sell price (SOL/token);
                    : Number(amount) /
                    Math.pow(10, 9) /;
                    (Number(amountOut) / Math.pow(10, token.tokenDecimals)), // Buy price (SOL/token),
                    txId: signature,
                    timestamp: new Date(),
                    };
                    const auto redisCache = getGlobalRedisCache();
                    const auto listKey = "swapsList:" + std::to_string(mintAddress);
                    try {
                        redisCache.lpush(listKey, JSON.stringify(swapRecord));
                        redisCache.ltrim(listKey, 0, MAX_SWAPS_TO_KEEP - 1);
                        logger.log(
                        "Helper: Saved swap to Redis list " + std::to_string(listKey) + " & trimmed. Type: " + std::to_string(direction == "0" ? "buy" : "sell")
                        );
                        } catch (redisError) {
                            logger.error(
                            "Helper: Failed to save swap to Redis list " + std::to_string(listKey) + ":"
                            redisError,
                            );
                            // Consider if we should proceed or return error
                        }

                        const auto newToken = db;
                        .update(tokens);
                        .set({
                            reserveAmount: Number(reserveToken),
                            reserveLamport: Number(reserveLamport),
                            currentPrice: currentPrice,
                            liquidity:
                            (Number(reserveLamport) / 1e9) * solPriceUSD +;
                            (Number(reserveToken) / Math.pow(10, token.tokenDecimals)) *;
                            tokenPriceUSD,
                            tokenPriceUSD,
                            solPriceUSD: solPriceUSD,
                            curveProgress:
                            ((Number(reserveLamport) - Number(process.env.VIRTUAL_RESERVES)) /;
                            (Number(process.env.CURVE_LIMIT) - Number(process.env.VIRTUAL_RESERVES))) *;
                            100,
                            txId: signature,
                            lastUpdated: new Date(),
                            "COALESCE(" + std::to_string(tokens.volume24h) + ", 0) + " + std::to_string(direction == "1)
                            ? (Number(amount) / Math.pow(10, token.tokenDecimals)) *;
                            tokenPriceUSD;
                            : (Number(amountOut) / Math.pow(10, token.tokenDecimals)) *
                            tokenPriceUSD;
                            }`,
                            priceChange24h,
                            // Conditionally set price24hAgo & lastPriceUpdate
                            ...(shouldReset24h;
                            ? {
                                price24hAgo: tokenPriceUSD,
                                lastPriceUpdate: now,
                            }
                            : {}),
                            });
                            .where(eq(tokens.mint, mintAddress));
                            .returning();
                            const auto { maxVolume, maxHolders } = getFeaturedMaxValues(db);

                            const auto enrichedToken = {;
                                ...newToken,
                                featuredScore: calculateFeaturedScore(
                                newToken[0],
                                maxVolume,
                                maxHolders,
                                ),
                                };
                                // Emit event to all clients via WebSocket
                                "token-" + std::to_string(mintAddress);
                                    ...swapRecord,
                                    mint: mintAddress, // Add mint field for compatibility
                                    timestamp: swapRecord.timestamp.toISOString(), // Emit ISO string
                                    });
                                    wsClient;
                                    "token-" + std::to_string(swapRecord.tokenMint);
                                    .emit("updateToken", enrichedToken);
                                    return {
                                        mintAddress,
                                        currentPrice,
                                        tokenPriceUSD,
                                        marketCapUSD,
                                        priceChange24h,
                                        };
                                    }
                                    } catch (e) {
                                        std::cerr << "Error processing swap log:" << e << std::endl;
                                        return;
                                    }

}

bool shouldUpdateSupply(const std::any& token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!token.lastSupplyUpdate) {
        return true;
    }
    const auto lastUpdate = new Date(token.lastSupplyUpdate).getTime();
    const auto oneHourAgo = Date.now() - 3600 * 1000;
    return lastUpdate < oneHourAgo;

}

std::future<> updateTokenSupplyFromChain(const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    tokenSupply: string;
    tokenSupplyUiAmount: number;
    tokenDecimals: number;
    lastSupplyUpdate: string;

}

std::future<bool> isValidSwapTx(Connection connection, const std::string& signature, const std::string& mint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto tx = connection.getTransaction(signature, {;
        maxSupportedTransactionVersion: 0,
        commitment: "confirmed",
        });
        const auto logs = tx.meta.logMessages;
        if (!logs) return false;

        const auto has = [&](kw: string) { return logs.some((l) => l.includes(kw)); };
        return (;
        has("Mint:") &&
        has("Swap:") &&
        has("Reserves:") &&
        has("Fee:") &&
        has("SwapEvent:")
        );

}

std::future<void> processLastValidSwap(const std::any& token, double solPriceUSD, auto limit) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto rpcUrl =;
    process.env.NETWORK == "devnet";
    ? process.env.DEVNET_SOLANA_RPC_URL;
    : process.env.MAINNET_SOLANA_RPC_URL;

    const auto connection = new Connection(rpcUrl || "", "confirmed");
    const auto mint = token.mint;
    if (!mint) {
        std::cerr << "processLastValidSwap: Token object missing mint property." << std::endl;
        return;
    }

    // Fetch the last `limit` signatures
    const auto sigs = connection.getSignaturesForAddress(new PublicKey(mint), {;
        limit,
        });

        // Iterate in order (most recent first)
        for (const int { signature } of sigs) {
            if (await isValidSwapTx(connection, signature, mint)) {
                // Found the most recent valid swap—process it once
                handleSignature(signature, token, solPriceUSD);
                return;
            }
        }

}

std::future<double> updateHoldersCache(const std::string& mint, bool imported = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto env = process.env;
    try {
        // Use the utility function to get the RPC URL with proper API key
        const auto connection = new Connection(process.env.NETWORK! == "devnet" ?;
        process.env.DEVNET_SOLANA_RPC_URL! : process.env.MAINNET_SOLANA_RPC_URL!,
        {
            commitment: "confirmed",
            confirmTransactionInitialTimeout: 60000, // 60 seconds
        }

        );

        const auto db = getDB();
        const auto existingToken = db;
        .select();
        .from(tokens);
        .where(eq(tokens.mint, mint));
        .limit(1);
        if (existingToken.[0].imported == 1) {
            //ignore imported tokens
            std::cout << "Token " + std::to_string(mint) << skipping holder update.` << std::endl;
            const auto redisCache = getGlobalRedisCache(); // Instantiate Redis cache;
            const auto ext = ExternalToken.create(mint, redisCache);
            ext.updateMarketAndHolders();
            return 0;
        }
        const auto redisCache = getGlobalRedisCache(); // Instantiate Redis cache;

        // Get all token accounts for this mint using getParsedProgramAccounts
        // This method is more reliable for finding all holders
        const auto accounts = connection.getParsedProgramAccounts(;
        new PublicKey("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"), // Token program;
        {
            filters: [
            {
                dataSize: 165, // Size of token account
                },
                {
                    memcmp: {
                        offset: 0,
                        bytes: mint, // Mint address
                        },
                        },
                        ],
                        },
                        );

                        if (!accounts || accounts.length == 0) {
                            std::cout << "No accounts found for token " + std::to_string(mint) << std::endl;
                            return 0;
                        }

                        std::cout << "Found " + std::to_string(accounts.length) + " token accounts for mint " + std::to_string(mint) << std::endl;

                        // Process accounts to extract holder information
                        auto totalTokens = 0;
                        // Change type from TokenHolder to any or a new local type if needed
                        const std::vector<std::any> holders = [];

                        // Process each account to get holder details
                        for (const auto& account : accounts)
                            try {
                                const auto parsedAccountInfo = account.account.data;
                                const auto tokenBalance =;
                                parsedAccountInfo.parsed.info.tokenAmount.uiAmount || 0;

                                // Skip accounts with zero balance
                                if (tokenBalance <= 0) continue;

                                const auto ownerAddress = parsedAccountInfo.parsed.info.owner || "";

                                // Skip accounts without owner
                                if (!ownerAddress) continue;

                                // Add to total tokens for percentage calculation
                                totalTokens += tokenBalance;

                                // Use a consistent structure, maybe matching old DB schema if needed
                                holders.push({
                                    // id: crypto.randomUUID(), // No longer needed for DB
                                    mint, // Keep for context within the stored object;
                                    address: ownerAddress,
                                    amount: tokenBalance,
                                    percentage: 0, // Will calculate after we have the total
                                    lastUpdated: new Date(), // Keep track of update time
                                    });



                                    } catch (error: any) {
                                        std::cerr << "Error processing account for " + std::to_string(mint) + ":" << error << std::endl;
                                        // Continue with other accounts even if one fails
                                        continue;
                                    }
                                }

                                // Calculate percentages now that we have the total
                                if (totalTokens > 0) {
                                    for (const auto& holder : holders)
                                        holder.percentage = (holder.amount / totalTokens) * 100;
                                    }
                                }

                                // Sort holders by amount (descending)
                                holders.sort((a, b) => b.amount - a.amount);

                                const auto holdersListKey = "holders:" + std::to_string(mint);
                                try {
                                    // Store the entire list, stringified. No TTL.
                                    redisCache.set(holdersListKey, JSON.stringify(holders));
                                    logger.log(
                                    "Stored " + std::to_string(holders.length) + " holders in Redis list " + std::to_string(holdersListKey)
                                    );
                                    } catch (redisError) {
                                        std::cerr << "Failed to store holders in Redis for " + std::to_string(mint) + ":" << redisError << std::endl;
                                    }

                                    try {
                                        const auto wsClient = getWebSocketClient();
                                        const auto limitedHolders = holders.slice(0, 50); // Emit only top 50;
                                        "token-" + std::to_string(mint);
                                        } catch (wsError) {
                                            std::cerr << "WebSocket error when emitting holder update:" << wsError << std::endl;
                                        }


                                        db;
                                        .update(tokens);
                                        .set({
                                            holderCount: holders.length, // Use full count
                                            lastUpdated: new Date(),
                                            });
                                            .where(eq(tokens.mint, mint));

                                            // Emit WebSocket event to notify of token update (with new holder count)
                                            try {
                                                const auto tokenData = db;
                                                .select();
                                                .from(tokens);
                                                .where(eq(tokens.mint, mint));
                                                .limit(1);

                                                if (tokenData && tokenData.length > 0) {
                                                    processTokenUpdateEvent({
                                                        ...tokenData[0],
                                                        event: "holdersUpdated",
                                                        holderCount: holders.length, // Use full count here too
                                                        timestamp: new Date().toISOString(),
                                                        });
                                                    }
                                                    } catch (wsError) {
                                                        std::cerr << "WebSocket error when emitting holder update: " + std::to_string(wsError) << std::endl;
                                                    }

                                                    return holders.length; // Return full count;
                                                    } catch (error) {
                                                        std::cerr << "Error updating holders for token " + std::to_string(mint) + ":" << error << std::endl;
                                                        return 0;
                                                    }

}

std::future<void> processTokenUpdateEvent(const std::any& tokenData, bool shouldEmitGlobal = false, bool isNewTokenEvent = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Get WebSocket client
            const auto wsClient = getWebSocketClient();

            // Get DB connection and calculate featuredScore
            const auto db = getDB();
            const auto { maxVolume, maxHolders } = getFeaturedMaxValues(db);

            // Create enriched token data with featuredScore
            const auto enrichedTokenData = {;
                ...tokenData,
                featuredScore: calculateFeaturedScore(
                tokenData,
                maxVolume,
                maxHolders,
                ),
                };

                // Always emit to token-specific room
                wsClient.emit(;
                "token-" + std::to_string(tokenData.mint)
                "updateToken",
                enrichedTokenData,
                );

                // Use env var for debug check
                const auto debugWs = process.env.DEBUG_WEBSOCKET == "true";
                if (debugWs) {
                    std::cout << "Emitted token update event for " + std::to_string(tokenData.mint) << std::endl;
                }

                // Handle global emission based on flags
                if (isNewTokenEvent) {
                    // If it's a new token event, *only* emit the global "newToken" event
                    wsClient.emit("global", "newToken", {
                        ...enrichedTokenData,
                        timestamp: new Date(),
                        });
                        if (debugWs) {
                            std::cout << "Emitted NEW token event to global feed: " + std::to_string(tokenData.mint) << std::endl;
                        }
                        } else if (shouldEmitGlobal) {
                            // Otherwise, if shouldEmitGlobal is true (and it's not a new token), emit "updateToken" globally
                            "token-" + std::to_string(tokenData.mint);
                                ...enrichedTokenData,
                                timestamp: new Date(),
                                });

                                if (debugWs) {
                                    std::cout << "Emitted token update event to global feed" << std::endl;
                                }
                            }

                            return;
                            } catch (error) {
                                std::cerr << "Error processing token update event:" << error << std::endl;
                                // Don't throw to avoid breaking other functionality
                            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
