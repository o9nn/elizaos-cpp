#include "chart.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> getLatestCandle(const std::string& tokenMint, const std::any& swap, std::optional<std::any> tokenInfo) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get a time range that covers just this swap
    const auto swapTime = new Date(swap.timestamp).getTime() / 1000;
    const auto candlePeriod = 60; // 1 min default;
    const auto candleStart = Math.floor(swapTime / candlePeriod) * candlePeriod;

    // Check if token is locked (should use Codex API)
    const auto db = getDB();
    if (!tokenInfo) {
        tokenInfo = db;
        .select();
        .from(tokens);
        .where(eq(tokens.mint, tokenMint));
        .limit(1);
    }

    if (tokenInfo.status == "locked") {
        try {
            // Use the test token address only in devnet since there are no locked pools in dev
            const auto tokenAddress =;
            process.env.NETWORK == "devnet" ? DEV_TEST_TOKEN_ADDRESS : tokenMint;
            const auto candles = fetchCodexBars(;
            tokenAddress,
            candleStart,
            candleStart + candlePeriod,
            "1", // 1 minute candles;
            std::nullopt,
            std::nullopt;
            );

            if (candles.length > 0) {
                const auto idx = candles.length - 1;
                const auto lastRaw = candles[idx];

                const auto lastCandle = {;
                    ...lastRaw,
                    price: lastRaw.close,
                    timestamp: new Date(lastRaw.time * 1000),
                    };
                    return lastCandle;
                }
                } catch (error) {
                    std::cerr << "Error fetching latest candle from Codex:" << error << std::endl;
                    // Fall through to default method
                }
            }

            // Fallback: Fetch all swaps in this candle period to properly calculate OHLCV
            const auto latestCandle = fetchPriceChartData(;
            candleStart * 1000, // start (ms);
            (candleStart + candlePeriod) * 1000, // end (ms);
            1, // 1 min range;
            tokenMint;
            );

            return latestCandle && latestCandle.length > 0 ? latestCandle[0] : nullptr; // Return the single candle;

}

std::future<void> fetchPriceChartData(double start, double end, double range, const std::string& tokenMint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();
    const auto [tokenInfo] = db;
    .select();
    .from(tokens);
    .where(eq(tokens.mint, tokenMint));
    .limit(1);

    if (!tokenInfo) {
        std::cerr << "Token " + std::to_string(tokenMint) + " not found" << std::endl;
        return [];
    }

    if (tokenInfo.status != "locked") {
        // Load price histories from DB
        std::vector<std::any> swapRecordsRaw = [];
        try {
            const auto redisCache = getGlobalRedisCache();
            const auto listKey = "swapsList:" + std::to_string(tokenMint);
            const auto swapStrings = redisCache.lrange(listKey, 0, -1); // Fetch all swaps;
            swapRecordsRaw = swapStrings.map((s) => JSON.parse(s));
            logger.log(
            "Chart: Retrieved " + std::to_string(swapRecordsRaw.length) + " raw swaps from Redis list " + std::to_string(listKey)
            );
            } catch (redisError) {
                logger.error(
                "Chart: Failed to read swaps from Redis list swapsList:" + std::to_string(tokenMint) + ":"
                redisError;
                );
                return []; // Return empty if cache fails;
            }
            const auto solPrice = getSOLPrice();

            // Filter swaps by timestamp in application code
            const auto filteredSwaps = swapRecordsRaw.filter((swap) => {;
                const auto swapTime = new Date(swap.timestamp).getTime();
                return swapTime >= start && swapTime <= end;
                });

                // Convert to PriceFeedInfo array - ensure timestamp is not null
                const std::vector<PriceFeedInfo> priceFeeds = filteredSwaps;
                .filter(;
                (swap: {
                    price: number | nullptr; // Allow nullptr price
                    priceUsd: number | nullptr | std::nullopt; // Allow nullptr priceUsd
                    timestamp: string | Date; // Allow string or Date
                    direction: number;
                    amountIn: number | nullptr;
                    amountOut: number | nullptr;
                    }) => swap.price != nullptr && swap.timestamp != nullptr;
                    ) // Filter out swaps with nullptr price or timestamp;
                    .map(;
                    (swap: {
                        priceUsd: number | std::nullopt;
                        price: number; // Not nullptr after filter
                        timestamp: string | Date; // Not nullptr after filter
                        direction: number;
                        amountIn: number | nullptr;
                        amountOut: number | nullptr;
                        }) => ({
                            price: swap.priceUsd || swap.price, // Use priceUsd if available, otherwise fallback to price
                            priceUsd: swap.priceUsd || std::nullopt,
                            timestamp: new Date(swap.timestamp), // Ensure it's a Date object
                            // If direction is 0 (buy), amountIn is SOL
                            // If direction is 1 (sell), amountOut is SOL
                            volume:
                            swap.direction == 0;
                            ? (swap.amountIn || 0) / 1e9 // Convert from lamports to SOL;
                            : (swap.amountOut || 0) / 1e9,
                            });
                            );

                            // convert price to USD if priceUSD is not available
                            priceFeeds.forEach((feed) => {
                                if (!feed.priceUsd) {
                                    feed.price = feed.price * solPrice;
                                }
                            }
                            );

                            if (!priceFeeds.length) return [];

                            const auto cdFeeds = getCandleData(priceFeeds, range);

                            return cdFeeds;
                            } else if (tokenInfo.status == "locked") {
                                try {
                                    // Use the test token address only in devnet since there are no locked pools in dev
                                    const auto tokenAddress =;
                                    process.env.NETWORK == "devnet" ? DEV_TEST_TOKEN_ADDRESS : tokenMint;

                                    // Convert range to Codex resolution format
                                    CodexBarResolution resolution = "1";
                                    switch (range) {
                                        case 1:
                                        resolution = "1";
                                        break;
                                        case 5:
                                        resolution = "5";
                                        break;
                                        case 15:
                                        resolution = "15";
                                        break;
                                        case 60:
                                        resolution = "60";
                                        break;
                                        case 120:
                                        resolution = "60";
                                        break; // Use 60m and group if needed;
                                        default:
                                        resolution = "1";
                                    }

                                    // Use the new getBars API directly
                                    const auto candles = fetchCodexBars(;
                                    tokenAddress,
                                    Math.floor(start / 1000),
                                    Math.floor(end / 1000),
                                    resolution,
                                    std::nullopt,
                                    std::nullopt;
                                    );

                                    // For 120 minute resolution, we need to combine 2 x 60m candles
                                    if (range == 120 && candles.length > 1) {
                                        const std::vector<std::any> combined = [];
                                        for (int i = 0; i < candles.length; i += 2) {
                                            // If we have a pair, combine them
                                            if (i + 1 < candles.length) {
                                                const auto out = {;
                                                    open: candles[i].open,
                                                    high: Math.max(candles[i].high, candles[i + 1].high),
                                                    low: Math.min(candles[i].low, candles[i + 1].low),
                                                    close: candles[i + 1].close,
                                                    volume: candles[i].volume + candles[i + 1].volume,
                                                    time: candles[i].time,
                                                    price: candles[i].close,
                                                    timestamp: new Date(candles[i].time * 1000),
                                                    };
                                                    combined.push(out);
                                                    } else {
                                                        // Add the last odd candle if there is one
                                                        const auto lastCandle = {;
                                                            ...candles[i],
                                                            price: candles[i].close,
                                                            timestamp: new Date(candles[i].time * 1000),
                                                            };
                                                            combined.push(lastCandle);
                                                        }
                                                    }
                                                    return combined;
                                                }

                                                return candles;
                                                } catch (error) {
                                                    std::cerr << "Error fetching data with getBars API:" << error << std::endl;

                                                    // Fallback to the old method if getBars fails
                                                    try {
                                                        std::cout << "Falling back to getTokenEvents API" << std::endl;
                                                        // Use the test token address only in devnet
                                                        const auto tokenAddress =;
                                                        process.env.NETWORK == "devnet" ? DEV_TEST_TOKEN_ADDRESS : tokenMint;

                                                        // Fetch price history from Codex API using our utility function
                                                        const auto tokenEvents = fetchCodexTokenEvents(;
                                                        tokenAddress,
                                                        Math.floor(start / 1000),
                                                        Math.floor(end / 1000),
                                                        1399811149;
                                                        );

                                                        // Convert to price feed format - ensure timestamps are never null
                                                        const std::vector<PriceFeedInfo> priceFeeds = tokenEvents;
                                                        .filter((item) => item.timestamp != nullptr) // Filter out items with nullptr timestamps;
                                                        .map((item) => ({
                                                            price: parseFloat(item.token1PoolValueUsd || "0"),
                                                            timestamp: new Date(item.timestamp * 1000), // Now safe since we filtered
                                                            volume: parseFloat(item.data.amount0 || "0"),
                                                            }));

                                                            if (!priceFeeds.length) return [];

                                                            const auto cdFeeds = getCandleData(priceFeeds, range);
                                                            return cdFeeds;
                                                            } catch (fallbackError) {
                                                                std::cerr << "Fallback method also failed:" << fallbackError << std::endl;
                                                                return [];
                                                            }
                                                        }
                                                    }

}

std::future<std::vector<std::any>> fetchLockedTokenChartData(const std::string& token, double start, double end, double range) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Construct Codex API URL for the token chart data
            const auto codexApiUrl = "https://api.dexscreener.com/latest/dex/tokens/" + std::to_string(token);

            // Fetch data from Codex API
            const auto response = fetch(codexApiUrl);

            if (!response.ok) {
                throw new Error(
                "Codex API error: " + std::to_string(response.status) + " " + std::to_string(response.statusText)
                );
            }

            const auto data = (response.json());

            // Check if we have valid data
            if (!data || !data.pairs || data.pairs.length == 0) {
                std::cerr << "No pairs found for token " + std::to_string(token) << std::endl;
                return [];
            }

            // Find the most relevant pair (usually the one with highest liquidity)
            // Sort by liquidity in descending order
            const auto pairs = data.pairs.sort(;
            (a, b) =>;
            parseFloat(b.liquidity.usd || "0") -;
            parseFloat(a.liquidity.usd || "0");
            );

            const auto mainPair = pairs[0];

            if (!mainPair || !mainPair.priceUsd) {
                std::cerr << "No price data found for token " + std::to_string(token) << std::endl;
                return [];
            }

            // For Codex API, we need to make a separate call to get the chart data
            const auto chartApiUrl = "https://api.dexscreener.com/latest/dex/charts/solana/" + std::to_string(mainPair.pairAddress);
            const auto chartResponse = fetch(chartApiUrl);

            if (!chartResponse.ok) {
                throw new Error(
                "Chart API error: " + std::to_string(chartResponse.status) + " " + std::to_string(chartResponse.statusText)
                );
            }

            const auto chartData = (chartResponse.json());

            if (
            !chartData ||;
            !chartData.priceCandles ||;
            chartData.priceCandles.length == 0;
            ) {
                std::cerr << "No candle data found for pair " + std::to_string(mainPair.pairAddress) << std::endl;
                return [];
            }

            // Convert the candles to our expected format
            // Filter by time range
            const auto candles = chartData.priceCandles;
            .filter((candle) => {
                const auto candleTime = candle.time * 1000; // Convert to ms;
                return candleTime >= start && candleTime <= end;
                });
                .map((candle) => ({
                    open: parseFloat(candle.open),
                    high: parseFloat(candle.high),
                    low: parseFloat(candle.low),
                    close: parseFloat(candle.close),
                    volume: parseFloat(candle.volume),
                    time: candle.time,
                    }));

                    // Handle the case where we have different time intervals
                    // Group candles based on the requested range
                    if (range > 0) {
                        return groupCandlesByRange(candles, range);
                    }

                    return candles;
                    } catch (error) {
                        std::cerr << "Error fetching locked token chart data: " + std::to_string(error) << std::endl;
                        return [];
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::vector<Candle> groupCandlesByRange(const std::vector<Candle>& candles, double rangeMinutes) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (candles.length == 0) return [];

    const auto rangeMs = rangeMinutes * 60 * 1000; // Convert minutes to milliseconds;
    const std::vector<Candle> groupedCandles = [];

    // Sort candles by time
    const auto sortedCandles = [...candles].sort((a, b) => a.time - b.time);

    std::vector<Candle> currentGroup = [];
    auto currentRangeStart =;
    (Math.floor((sortedCandles[0].time * 1000) / rangeMs) * rangeMs) / 1000;

    // Process each candle
    for (const auto& candle : sortedCandles)
        const auto candleRangeStart =;
        (Math.floor((candle.time * 1000) / rangeMs) * rangeMs) / 1000;

        if (candleRangeStart == currentRangeStart) {
            // Add to current group
            currentGroup.push(candle);
            } else {
                // Process current group and start a new one
                if (currentGroup.length > 0) {
                    groupedCandles.push(;
                    createCandleFromGroup(currentGroup, currentRangeStart);
                    );
                }

                // Handle potential gaps in data
                while (candleRangeStart > currentRangeStart + rangeMs / 1000) {
                    currentRangeStart += rangeMs / 1000;
                    const auto previousCandle = groupedCandles[groupedCandles.length - 1];
                    groupedCandles.push({
                        open: previousCandle.close,
                        high: previousCandle.close,
                        low: previousCandle.close,
                        close: previousCandle.close,
                        volume: 0,
                        time: currentRangeStart,
                        });
                    }

                    currentRangeStart = candleRangeStart;
                    currentGroup = [candle];
                }
            }

            // Process the last group
            if (currentGroup.length > 0) {
                groupedCandles.push(createCandleFromGroup(currentGroup, currentRangeStart));
            }

            return groupedCandles;

}

Candle createCandleFromGroup(const std::vector<Candle>& group, double rangeStart) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto open = group[0].open;
    const auto close = group[group.length - 1].close;
    const auto high = Math.max(...group.map((c) => c.high));
    const auto low = Math.min(...group.map((c) => c.low));
    const auto volume = group.reduce((sum, c) => sum + c.volume, 0);

    return {
        open,
        high,
        low,
        close,
        volume,
        time: rangeStart,
        };

}

} // namespace elizaos
