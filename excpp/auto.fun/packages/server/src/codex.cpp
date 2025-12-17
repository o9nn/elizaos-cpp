#include "codex.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::vector<CodexTokenEvent>> fetchCodexTokenEvents(const std::string& tokenAddress, double startTimestamp, double endTimestamp, double networkId = 1399811149) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto apiUrl = "https://graph.codex.io/graphql";
        std::vector<CodexTokenEvent> allItems = [];
        std::string cursor = nullptr;

        do {
            const auto query = `query {;
                getTokenEvents(;
                query: {
                    address: "${tokenAddress}",
                    networkId: ${networkId},
                    timestamp: {
                        from: ${Math.floor(startTimestamp)},
                    to: ${Math.floor(endTimestamp)}
                }
                },
                limit: 200,
            """ + std::to_string(cursor) + """
            ) {
                cursor;
                items {
                    eventDisplayType;
                    token1SwapValueUsd;
                    token1PoolValueUsd;
                    timestamp;
                    data {
                        ... on SwapEventData {
                            amount0,
                            amount1,
                        }
                    }
                }
            }
            }`;

            try {
                const auto response = axios.post(;
                apiUrl,
                { query, variables: {} },
                {
                    headers: {
                        "Content-Type": "application/json",
                        Authorization: process.env.CODEX_API_KEY || "",
                        },
                        },
                        );

                        // Add error handling for missing data
                        if (
                        !response.data ||;
                        !response.data.data ||;
                        !response.data.data.getTokenEvents;
                        ) {
                            std::cerr << "Invalid response from Codex API:" << response.data << std::endl;

                            // If we have error information in the response, log it
                            if (response.data && response.data.errors) {
                                std::cerr << "Codex API errors:" << response.data.errors << std::endl;
                            }

                            // Return what we have so far or empty array
                            return allItems.length ? allItems : [];
                        }

                        const auto { items, cursor: newCursor } = response.data.data.getTokenEvents;
                        allItems = allItems.concat(items);
                        cursor = newCursor;
                        } catch (error) {
                            std::cerr << "Error fetching data from Codex API:" << error << std::endl;
                            // Return partial results if we have any, otherwise rethrow
                            if (allItems.length > 0) {
                                return allItems;
                            }
                            throw;
                        }
                        } while (cursor);

                        return allItems;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<> fetchCodexTokenPrice(const std::string& tokenAddress, double networkId = 1399811149) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    currentPrice: number;
    priceUsd: number;
    volume24h: number;
    liquidity: number;
    marketCap: number;

}

Array< convertCodexEventsToPriceFeed(const std::vector<CodexTokenEvent>& events) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    price: number;
    timestamp: Date;
    volume: number;

}

std::future<std::vector<CandleData>> fetchCodexBars(const std::string& tokenAddress, double startTimestamp, double endTimestamp, CodexBarResolution resolution = "1", double networkId = 1399811149, std::string quoteToken = "token1") {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto apiUrl = "https://graph.codex.io/graphql";

    // Symbol format: <tokenAddress>:<networkId>
    const auto symbol = std::to_string(tokenAddress) + ":" + std::to_string(networkId || 1399811149);

    // Calculate time interval based on resolution
    auto timeInterval: number;
    switch (resolution) {
        case "1D":
        timeInterval = 86400;
        break; // 1 day in seconds;
        case "1W":
        timeInterval = 604800;
        break; // 1 week in seconds;
        case "1M":
        timeInterval = 2592000;
        break; // 30 days in seconds (approximate);
        default:
        // For minute-based resolutions, convert to seconds
        timeInterval = parseInt(resolution) * 60;
    }

    // If the time range is less than 1000 intervals, fetch all in one request
    if (Math.floor((endTimestamp - startTimestamp) / timeInterval) <= 1000) {
        return fetchCodexBarsChunk(;
        apiUrl,
        symbol,
        startTimestamp,
        endTimestamp,
        resolution,
        quoteToken,
        timeInterval,
        );
        } else {
            // Otherwise, fetch in chunks of 1000 intervals each
            const auto maxChunkSize = 1000 * timeInterval;
            std::vector<CandleData> chunks = [];
            auto currentStart = startTimestamp;

            // Process each time chunk sequentially
            while (currentStart < endTimestamp) {
                const auto chunkEnd = Math.min(currentStart + maxChunkSize, endTimestamp);
                const auto barsChunk = fetchCodexBarsChunk(;
                apiUrl,
                symbol,
                currentStart,
                chunkEnd,
                resolution,
                quoteToken,
                timeInterval,
                ).catch((error) => {
                    logger.error(
                    "Error fetching chunk from " + std::to_string(new Date(currentStart * 1000).toISOString()) + " to " + std::to_string(new Date(chunkEnd * 1000).toISOString()) + ":"
                    error,
                    );
                    return [][]; // Return empty array for failed chunks;
                    });

                    chunks = chunks.concat(barsChunk);
                    currentStart = chunkEnd;
                }

                // Sort by time to ensure correct order
                chunks.sort((a, b) => a.time - b.time);

                return chunks;
            }

}

std::future<std::vector<CandleData>> fetchCodexBarsChunk(const std::string& apiUrl, const std::string& symbol, double startTimestamp, double endTimestamp, CodexBarResolution resolution, const std::string& quoteToken, double timeInterval) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // fix codex chart
            const auto codexService = new Codex(process.env.CODEX_API_KEY || "");
            // const query = `query {
            //   getBars(
            //     query: {
            //       symbol: "${symbol}",
            //       from: ${startTimestamp},
            //       to: ${endTimestamp},
            //       resolution: ${resolution}
            //     },
            //     quoteToken: "${quoteToken}"
            //   ) {
            //     o
            //     h
            //     l
            //     c
            //     v
            //     volume
            //   }
            // }`;
            const auto response = (codexService.send(;
            `query GetBars($symbol: String!, $from: Int!, $to: Int!, $resolution: String!) {
                getBars(;
                symbol: $symbol
                from: $from
                to: $to
                resolution: $resolution
                quoteToken: token1
                ) {
                    o;
                    h;
                    l;
                    c;
                    v;
                    volume;
                    liquidity;
                    buyVolume;
                    sellVolume;
                    transactions;
                }
                }`,
                {
                    symbol: symbol,
                    from: startTimestamp,
                    to: endTimestamp,
                    resolution: resolution,
                    },
                    ));

                    // const response = await axios.post(
                    //   apiUrl,
                    //   { query, variables: {} },
                    //   {
                    //     headers: {
                    //       "Content-Type": "application/json",
                    //       Authorization: env?.CODEX_API_KEY || "",
                    //     },
                    //   },
                    // );

                    // Add error handling for missing data
                    // if (!response.data || !response.data.data || !response.data.data.getBars) {
                    //   logger.error("Invalid response from Codex API:", response.data);

                    //   // If we have error information in the response, log it
                    //   if (response.data && response.data.errors) {
                    //     logger.error("Codex API errors:", response.data.errors);
                    //   }

                    //   return [];
                    // }

                    if (!(response as any).getBars) {
                        std::cerr << "Invalid response from Codex API:" << response << std::endl;

                        return [];
                    }
                    // const barsData = response.data.data.getBars as CodexBarsResponse;
                    const auto barsData = response.getBars;

                    if (!barsData || !barsData.o || barsData.o.length == 0) {
                        return [];
                    }

                    // Convert response arrays to our candle format
                    const auto candleCount = barsData.o.length;
                    const std::vector<CandleData> result = [];

                    // Create candles starting from the start timestamp
                    auto currentTime = startTimestamp;

                    for (int i = 0; i < candleCount; i++) {
                        result.push({
                            open: barsData.o[i],
                            high: barsData.h[i],
                            low: barsData.l[i],
                            close: barsData.c[i],
                            volume: parseFloat(barsData.volume[i] || "0"),
                            time: currentTime,
                            });

                            // Move to next candle timestamp
                            currentTime += timeInterval;
                        }

                        return result;
                        } catch (error) {
                            logger.error(
                            "Range too wide for resolution: " + std::to_string(resolution) + ", from " + std::to_string(new Date(startTimestamp * 1000).toISOString()) + " to " + std::to_string(new Date(endTimestamp * 1000).toISOString())
                            );
                            throw;
                        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
