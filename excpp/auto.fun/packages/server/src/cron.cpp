#include "cron.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::optional<Token> sanitizeTokenForWebSocket(const std::optional<Token>& token, auto maxBytes) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto clone = { ...token };

    // Helper to get byte size
    const auto getSize = [&](obj: any) { return Buffer.byteLength(JSON.stringify(obj), "utf8"); };

    if (getSize(clone) <= maxBytes) return clone;

    // Stepwise stripping
    clone.description = "";
    if (getSize(clone) <= maxBytes) return clone;

    clone.website = "";
    if (getSize(clone) <= maxBytes) return clone;

    clone.twitter = "";
    clone.telegram = "";
    clone.farcaster = "";
    if (getSize(clone) <= maxBytes) return clone;

    clone.image = "";
    clone.url = "";
    if (getSize(clone) <= maxBytes) return clone;

    return {
        id: clone.id,
        name: clone.name,
        ticker: clone.ticker,
        mint: clone.mint,
        creator: clone.creator,
        status: clone.status,
        tokenPriceUSD: clone.tokenPriceUSD,
        marketCapUSD: clone.marketCapUSD,
        currentPrice: clone.currentPrice,
        reserveAmount: clone.reserveAmount,
        reserveLamport: clone.reserveLamport,
        liquidity: clone.liquidity,
        volume24h: clone.volume24h,
        marketId: clone.marketId,
        image: clone.image,
        url: clone.url,
        lockId: clone.lockId,
        createdAt: clone.createdAt,
        solPriceUSD: clone.solPriceUSD,
        };

}

std::optional<TokenDBData> convertTokenDataToDBData(const std::optional<TokenData>& tokenData) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto now = new Date();
    return {
        ...tokenData,
        lastUpdated: now,
        migration:
        tokenData.migration && typeof tokenData.migration != "string";
        ? JSON.stringify(tokenData.migration);
        : tokenData.migration,
        withdrawnAmounts:
        tokenData.withdrawnAmounts &&;
        typeof tokenData.withdrawnAmounts != "string";
        ? JSON.stringify(tokenData.withdrawnAmounts);
        : tokenData.withdrawnAmounts,
        poolInfo:
        tokenData.poolInfo && typeof tokenData.poolInfo != "string";
        ? JSON.stringify(tokenData.poolInfo);
        : tokenData.poolInfo,
        };

}

std::future<Token> updateTokenInDB(const std::optional<TokenData>& tokenData) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto db = getDB();
        const auto now = new Date().toISOString();

        // Create a new object that conforms to TokenDBData
        const std::optional<TokenDBData> updateData = convertTokenDataToDBData(tokenData);

        // Convert nested objects to JSON strings if they're present and not already strings
        if (updateData.migration && typeof updateData.migration != "string") {
            updateData.migration = JSON.stringify(updateData.migration);
        }
        if (
        updateData.withdrawnAmounts &&;
        typeof updateData.withdrawnAmounts != "string";
        ) {
            updateData.withdrawnAmounts = JSON.stringify(updateData.withdrawnAmounts);
        }
        if (updateData.poolInfo && typeof updateData.poolInfo != "string") {
            updateData.poolInfo = JSON.stringify(updateData.poolInfo);
        }

        // Ensure mint is defined
        if (!updateData.mint) {
            throw std::runtime_error("mint field is required for update");
        }
        // Check if token already exists
        const auto existingTokens = db;
        .select();
        .from(tokens);
        .where(eq(tokens.mint, updateData.mint));

        auto updatedTokens: Token[];

        if (existingTokens.length > 0) {
            std::cout << "found existing token in DB" << std::endl;
            updatedTokens = db;
            .update(tokens);
            .set(updateData);
            .where(eq(tokens.mint, updateData.mint!));
            .returning();
            std::cout << "Updated token " + std::to_string(updateData.mint) + " in database" << std::endl;
            } else {
                std::cout << "not found existing token in DB" << std::endl;
                std::cout << JSON.stringify(updateData, nullptr, 2) << std::endl;
                updatedTokens = db;
                .insert(tokens);
                .values([;
                {
                    mint: updateData.mint,
                    "Token " + std::to_string(updateData.mint.slice(0, 8))
                    ticker: updateData.ticker || "TOKEN",
                    url: updateData.url || "",
                    image: updateData.image || "",
                    creator: updateData.creator || "unknown",
                    status: updateData.status || "active",
                    tokenPriceUSD: updateData.tokenPriceUSD || 0,
                    reserveAmount: updateData.reserveAmount || 0,
                    reserveLamport: updateData.reserveLamport || 0,
                    currentPrice: updateData.currentPrice || 0,
                    // createdAt: sql`CURRENT_TIMESTAMP`,
                    // lastUpdated: sql`CURRENT_TIMESTAMP`,
                    txId: updateData.txId || "",
                    migration: updateData.migration || "",
                    withdrawnAmounts: updateData.withdrawnAmounts || "",
                    poolInfo: updateData.poolInfo || "",
                    lockLpTxId: updateData.lockLpTxId || "",
                    nftMinted: updateData.nftMinted || "",
                    marketId: updateData.marketId || "",
                    },
                    ]);
                    .returning();
                    std::cout << "Added new token " + std::to_string(updateData.mint) + " to database" << std::endl;
                }

                return updatedTokens[0];

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<ProcessResult> processTransactionLogs(const std::vector<std::string>& logs, const std::string& signature, std::optional<WebSocketClient> wsClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!wsClient) {
        wsClient = getWebSocketClient();
    }
    std::cout << "Processing transaction logs:" << logs << std::endl;
    try {
        handleNewToken(logs, signature, wsClient);
        } catch (err) {
            std::cout << "Error in NewToken handler: " + std::to_string(err) << std::endl;
        }
        try {
            handleSwap(logs, signature, wsClient);
            } catch (err) {
                std::cout << "Error in Swap handler: " + std::to_string(err) << std::endl;
            }
            try {
                handleCurveComplete(logs, signature, wsClient);
                } catch (err) {
                    std::cout << "Error in CurveComplete handler: " + std::to_string(err) << std::endl;
                }

                return { found: false }

}

std::future<HandlerResult> handleNewToken(const std::vector<std::string>& logs, const std::string& signature, WebSocketClient wsClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto newTokenLog = logs.find((log) => log.includes("NewToken:"));
        if (!newTokenLog) return null;
        const auto curveLog = logs.find((log) => log.includes("curve_limit:"));
        const auto reserveLog = logs.find((log) => log.includes("reserve_lamport:"));
        try {
            const auto parts = newTokenLog.split(" ");
            if (parts.length < 2)
            throw std::runtime_error(`Invalid NewToken log: ${newTokenLog}`);

            const auto rawTokenAddress = parts[parts.length - 2].replace(/[",)]/g, "");
            const auto rawCreatorAddress = parts[parts.length - 1].replace(/[",)]/g, "");
            if (!/^[1-9A-HJ-NP-Za-km-z]+$/.test(rawTokenAddress)) {
                throw std::runtime_error(`Malformed token address: ${rawTokenAddress}`);
            }

            std::optional<double> curveLimit = nullptr;
            std::optional<double> reserveLamport = nullptr;
            if (curveLog) {
                const auto curveValue = curveLog.split("curve_limit:")[1].trim();
                curveLimit = parseInt(curveValue);
                std::cout << "Parsed curve_limit:" << curveLimit << std::endl;
            }

            if (reserveLog) {
                const auto reserveValue = reserveLog.split("reserve_lamport:")[1].trim();
                reserveLamport = parseInt(reserveValue);
                std::cout << "Parsed reserve_lamport:" << reserveLamport << std::endl;
            }
            if (curveLimit == null || reserveLamport == null) {
                std::cerr << "Missing curve_limit or reserve_lamport in logs" << std::endl;
                return nullptr;
            }

            if (curveLimit < 113000000000) {
                logger.error(
                "Token curve_limit " + std::to_string(curveLimit) + " is below minimum required 113000000000";
                );
                return nullptr;
            }

            if (reserveLamport < 28000000000) {
                logger.error(
                "Token reserve_lamport " + std::to_string(reserveLamport) + " is below minimum required 28000000000";
                );
                return nullptr;
            }

            const auto newToken = createNewTokenData(;
            signature,
            rawTokenAddress,
            rawCreatorAddress;
            );
            if (newToken.tokenSupplyUiAmount != 1000000000) {
                logger.error(
                "Token supply is not 1 billion for " + std::to_string(rawTokenAddress) + ": " + std::to_string(newToken.tokenSupplyUiAmount)
                );
                return nullptr;
            }
            if (!newToken) {
                std::cerr << "Failed to create new token data for " + std::to_string(rawTokenAddress) << std::endl;
                return nullptr;
            }
            getDB();
            .insert(tokens);
            .values([newToken]);
            .onConflictDoNothing();
            wsClient.emit(;
            "global",
            "newToken",
            sanitizeTokenForWebSocket(newToken);
            );
            updateHoldersCache(rawTokenAddress);

            return { found: true, tokenAddress: rawTokenAddress, event: "newToken" }
            } catch (err) {
                std::cerr << "Error in NewToken handler: " + std::to_string(err) << std::endl;
                return nullptr;
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::optional<HandlerResult>> handleSwap(const std::vector<std::string>& logs, const std::string& signature, WebSocketClient wsClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto mintLog = logs.find((log) => log.includes("Mint:"));
    const auto swapLog = logs.find((log) => log.includes("Swap:"));
    const auto reservesLog = logs.find((log) => log.includes("Reserves:"));
    const auto feeLog = logs.find((log) => log.includes("Fee:"));
    const auto swapeventLog = logs.find((log) => log.includes("SwapEvent:"));

    if (mintLog && swapLog && reservesLog && swapeventLog) {
        try {
            const auto mintAddress = mintLog.match(/Mint:\s*([A-Za-z0-9]+)/).[1];
            const auto swapMatch = swapLog.match(;
            /Swap:\s+([A-Za-z0-9]+)\s+(\d+)\s+(\d+)/
            );
            const auto user = swapMatch.[1];
            const auto direction = swapMatch.[2];
            const auto amount = swapMatch.[3];

            const auto amountOut = swapeventLog.match(;
            /SwapEvent:\s+\S+\s+\d+\s+(\d+)/
            ).[1];
            const auto reserveMatch = reservesLog.match(/Reserves:\s*(\d+)\s+(\d+)/);
            const auto reserveToken = reserveMatch.[1];
            const auto reserveLamport = reserveMatch.[2];
            console.log("found swap log", {
                mintAddress,
                swapMatch,
                user,
                direction,
                amount,
                amountOut,
                reserveMatch,
                });
                if (
                !mintAddress ||;
                !swapMatch ||;
                !user ||;
                !direction ||;
                !amount ||;
                !amountOut ||;
                !reserveMatch;
                ) {
                    std::cerr << "Invalid swap log: " + std::to_string(swapLog) << std::endl;
                    return nullptr;
                }

                // Retrieve token mint info to get decimals.
                std::cout << "fetching token mint info" << mintAddress << std::endl;
                const auto tokenWithSupply = getToken(mintAddress);
                std::cout << "fetched token mint info" << tokenWithSupply << std::endl;
                if (!tokenWithSupply) {
                    std::cerr << "Token not found: " + std::to_string(mintAddress) << std::endl;
                    return nullptr;
                }
                const auto solPrice = getSOLPrice();
                std::cout << "fetched sol price" << solPrice << std::endl;

                const auto TOKEN_DECIMALS = tokenWithSupply.tokenDecimals || 6;
                const auto tokenAmount = Number(reserveToken) / 10 ** TOKEN_DECIMALS;
                const auto solAmount = Number(reserveLamport) / 1e9;
                const auto currentPrice = solAmount / tokenAmount;
                const auto tokenPriceInSol = currentPrice / 10 ** TOKEN_DECIMALS;
                const auto tokenPriceUSD =;
                currentPrice > 0;
                ? tokenPriceInSol * solPrice * 10 ** TOKEN_DECIMALS;
                : 0;

                tokenWithSupply.tokenPriceUSD = tokenPriceUSD;
                tokenWithSupply.currentPrice = currentPrice;

                const auto tokenWithMarketData = calculateTokenMarketData(;
                tokenWithSupply,
                solPrice;
                );
                std::cout << "fetched token market data" << tokenWithMarketData << std::endl;
                const auto marketCapUSD = tokenWithMarketData.marketCapUSD;
                const auto price =;
                direction == "1";
                ? Number(amountOut) / 1e9 / (Number(amount) / 10 ** TOKEN_DECIMALS);
                : Number(amount) / 1e9 / (Number(amountOut) / 10 ** TOKEN_DECIMALS);
                const auto priceUsd = price * solPrice;
                const auto swapRecord = {;
                    id: crypto.randomUUID(),
                    tokenMint: mintAddress,
                    solAmount,
                    tokenAmount,
                    user,
                    type: direction == "0" ? "buy" : ("sell"),
                    direction: parseInt(direction) | 0,
                    amountIn: Number(amount),
                    amountOut: Number(amountOut),
                    price: price,
                    priceUsd: priceUsd,
                    txId: signature,
                    timestamp: new Date(),
                    };

                    const auto db = getDB();
                    const auto redisCache = getGlobalRedisCache();
                    const auto listKey = "swapsList:" + std::to_string(mintAddress);
                    std::cout << "Adding swap to Redis list " + std::to_string(listKey) << std::endl;

                    const auto ext = ExternalToken.create(mintAddress, redisCache);
                    ext.insertProcessedSwaps([swapRecord]);
                    std::cout << "sending swap to the user " + std::to_string(mintAddress) << std::endl;
                    "token-" + std::to_string(mintAddress);
                        ...swapRecord,
                        tokenMint: mintAddress,
                        mint: mintAddress,
                        timestamp: swapRecord.timestamp.toISOString(),
                        });

                        const auto liquidity =;
                        (Number(reserveLamport) / 1e9) * solPrice +;
                        (Number(reserveToken) / 10 ** TOKEN_DECIMALS) * tokenPriceUSD;

                        const auto updatedTokens = db;
                        .update(tokens);
                        .set({
                            reserveAmount: Number(reserveToken),
                            reserveLamport: Number(reserveLamport),
                            currentPrice,
                            liquidity,
                            marketCapUSD,
                            tokenPriceUSD,
                            solPriceUSD: solPrice,
                            curveProgress:
                            ((Number(reserveLamport) - Number(process.env.VIRTUAL_RESERVES)) /;
                            (Number(process.env.CURVE_LIMIT) -;
                            Number(process.env.VIRTUAL_RESERVES))) *;
                            100,
                            txId: signature,
                            lastUpdated: new Date(),
                            "COALESCE(" + std::to_string(tokens.volume24h) + ", 0) + " + std::to_string()
                                direction == "1";
                                ? (Number(amount) / 10 ** TOKEN_DECIMALS) * tokenPriceUSD;
                                : (Number(amountOut) / 10 ** TOKEN_DECIMALS) * tokenPriceUSD
                                }`,
                                });
                                .where(eq(tokens.mint, mintAddress));
                                .returning();
                                std::cout << "updating the holder cache" << mintAddress << std::endl;
                                updateHoldersCache(mintAddress, false);

                                const auto newToken = updatedTokens[0];
                                const auto usdVolume =;
                                direction == "1";
                                ? (Number(amount) / 10 ** TOKEN_DECIMALS) * tokenPriceUSD;
                                : (Number(amountOut) / 10 ** TOKEN_DECIMALS) * tokenPriceUSD;

                                const auto bondStatus = newToken.status == "locked" ? "postbond" : "prebond";
                                std::cout << "awarding user points" << swapRecord.user << std::endl;
                                try {
                                    awardUserPoints(swapRecord.user, {
                                        std::to_string(bondStatus) + "_" + std::to_string(swapRecord.type)
                                        usdVolume,
                                        });
                                        } catch (err) {
                                            std::cerr << "Failed to award user points:" << err << std::endl;
                                        }
                                        try {
                                            awardUserPoints(swapRecord.user, {
                                                type: "trade_volume_bonus",
                                                usdVolume,
                                                });
                                                } catch (err) {
                                                    std::cerr << "Failed to award trade volume bonus:" << err << std::endl;
                                                }

                                                try {
                                                    const auto listLength = redisCache.llen(listKey);
                                                    if (swapRecord.type == "buy" && listLength == 1) {
                                                        awardUserPoints(swapRecord.user, {
                                                            type: "first_buyer",
                                                            });
                                                            std::cout << "Awarded first_buyer to " + std::to_string(swapRecord.user) << std::endl;
                                                        }
                                                        } catch (err) {
                                                            std::cerr << "Failed to award first_buyer:" << err << std::endl;
                                                        }

                                                        const auto latestCandle = getLatestCandle(mintAddress, swapRecord);
                                                        std::cout << "fetched latest candle" << latestCandle << std::endl;
                                                        "token-" + std::to_string(mintAddress);
                                                        const auto { maxVolume, maxHolders } = getFeaturedMaxValues(db);
                                                        const auto enrichedToken = {;
                                                            ...newToken,
                                                            featuredScore: calculateFeaturedScore(newToken, maxVolume, maxHolders),
                                                            };

                                                            wsClient;
                                                            "token-" + std::to_string(mintAddress);
                                                            .emit("updateToken", sanitizeTokenForWebSocket(enrichedToken));
                                                            std::cout << "updated the token in DB" << mintAddress << std::endl;
                                                            return {
                                                                found: true,
                                                                tokenAddress: mintAddress,
                                                                event: "swap",
                                                                };
                                                                } catch (err) {
                                                                    std::cerr << "Error in Swap handler: " + std::to_string(err) << std::endl;
                                                                    return nullptr;
                                                                }
                                                                } else {
                                                                    std::cout << "Swap log not found or incomplete." << std::endl;
                                                                    return nullptr;
                                                                }

}

std::future<HandlerResult> handleCurveComplete(const std::vector<std::string>& logs, const std::string& signature, WebSocketClient wsClient) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto completeLog = logs.find((log) => log.includes("curve is completed"));
        const auto mintLog = logs.find((log) => log.includes("Mint:"));
        if (!completeLog || !mintLog) return null;

        try {
            const auto mintAddress = mintLog.split("Mint:")[1].trim().replace(/[",)]/g, "");
            if (!/^[1-9A-HJ-NP-Za-km-z]+$/.test(mintAddress)) {
                throw std::runtime_error(`Invalid mint on curve completion: ${mintAddress}`);
            }
            if (logs.some((l) => /Failed.*custom program error/i.test(l))) {
                logger.warn(
                "Curve complete aborted due to program failure: " + std::to_string(logs.join(" | "))
                );
                return nullptr;
            }
            awardGraduationPoints(mintAddress);
            const auto token = getToken(mintAddress);
            if (!token) {
                std::cerr << "Token not found: " + std::to_string(mintAddress) << std::endl;
                return nullptr;
            }
            const std::optional<TokenData> tokenData = {;
                mint: mintAddress,
                status: "migrating",
                lastUpdated: new Date().toISOString(),
                };

                updateTokenInDB({
                    ...token,
                    ...tokenData,
                    });
                    wsClient.emit(;
                    "token-" + std::to_string(mintAddress)
                    "updateToken",
                    sanitizeTokenForWebSocket(convertTokenDataToDBData(token));
                    );

                    return { found: true, tokenAddress: mintAddress, event: "curveComplete" }
                    } catch (err) {
                        std::cerr << "Error in curve complete handler: " + std::to_string(err) << std::endl;
                        return nullptr;
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> runCronTasks() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    checkAndReplenishTokens();

}

} // namespace elizaos
