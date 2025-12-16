#include "util.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;

struct CodexTokenEvent {
    std::string eventDisplayType;
    std::string token1SwapValueUsd;
    std::string token1PoolValueUsd;
    double timestamp;
    { data;
    std::optional<std::string> amount0;
    std::optional<std::string> amount1;
};


/**
 * Fetches token events from the Codex API
 * @param tokenAddress Token address to fetch events for
 * @param startTimestamp Start timestamp in seconds
 * @param endTimestamp End timestamp in seconds
 * @param networkId Network ID (default: 1399811149 for Solana)
 * @param env Environment variables containing CODEX_API_KEY
 * @returns Array of token events
 */
std::future<std::vector<CodexTokenEvent>> fetchCodexTokenEvents(const std::string& tokenAddress, double startTimestamp, double endTimestamp, number = 1399811149 networkId);", 
          networkId: ${networkId}, 
          timestamp: {
            from: ${Math.floor(startTimestamp)},
            to: ${Math.floor(endTimestamp)}
          }
        },
        limit: 200,
        cursor: ${cursor ? `"${cursor}"` : null}
      ) {
        cursor
        items {
          eventDisplayType
          token1SwapValueUsd
          token1PoolValueUsd
          timestamp
          data {
            ... on SwapEventData {
              amount0,
              amount1,
            }
          }
        }
      }
    }` as string;

    try {
      const response = await axios.post(
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
        !response.data ||
        !response.data.data ||
        !response.data.data.getTokenEvents
      ) {
        logger.error("Invalid response from Codex API:", response.data);

        // If we have error information in the response, log it
        if (response.data && response.data.errors) {
          logger.error("Codex API errors:", response.data.errors);
        }

        // Return what we have so far or empty array
        return allItems.length ? allItems : [];
      }

      const { items, cursor: newCursor } = response.data.data.getTokenEvents;
      allItems = allItems.concat(items);
      cursor = newCursor;
    } catch (error) {
      logger.error("Error fetching data from Codex API:", error);
      // Return partial results if we have any, otherwise rethrow
      if (allItems.length > 0) {
        return allItems;
      }
      throw error;
    }
  } while (cursor);

  return allItems;
}

/**
 * Fetches current token price and market data from Codex API
 * @param tokenAddress Token address to fetch price for
 * @param networkId Network ID (default: 1399811149 for Solana)
 * @param env Environment variables containing CODEX_API_KEY
 * @returns Object with current price and market data
 */
std::future<{
  currentPrice: number;
  priceUsd: number;
  volume24h: number;
  liquidity: number;
  marketCap: number;
}> fetchCodexTokenPrice(const std::string& tokenAddress, number = 1399811149 networkId);",
        networkId: ${networkId}
      }
    ) {
      items {
        liquidity
        volume24h
        price
        priceUsd
        totalMarketCap
      }
    }
  }`;

  try {
    const response = await axios.post(
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
      !response.data ||
      !response.data.data ||
      !response.data.data.getTokens ||
      !response.data.data.getTokens.items ||
      response.data.data.getTokens.items.length === 0
    ) {
      logger.error(
        `Token ${tokenAddress} not found or invalid response from Codex API:`,
        response.data,
      );

      // If we have error information in the response, log it
      if (response.data && response.data.errors) {
        logger.error("Codex API errors:", response.data.errors);
      }

      // Return default values
      return {
        currentPrice: 0,
        priceUsd: 0,
        volume24h: 0,
        liquidity: 0,
        marketCap: 0,
      };
    }

    const token = response.data.data.getTokens.items[0];

    return {
      currentPrice: parseFloat(token.price || "0"),
      priceUsd: parseFloat(token.priceUsd || "0"),
      volume24h: parseFloat(token.volume24h || "0"),
      liquidity: parseFloat(token.liquidity || "0"),
      marketCap: parseFloat(token.totalMarketCap || "0"),
    };
  } catch (error) {
    logger.error(
      `Error fetching token price from Codex API for ${tokenAddress}:`,
      error,
    );
    throw error;
  }
}

/**
 * Converts Codex token events to price feed format
 * @param events Array of Codex token events
 * @returns Array of price feed objects
 */
> {
  return events.map((item) => ({
    price: parseFloat(item.token1PoolValueUsd),
    timestamp: new Date(item.timestamp * 1000),
    volume: parseFloat(item.data.amount0 || "0"),
  }));
}

/**
 * Resolution type for Codex getBars API
 * Possible values: '1', '5', '15', '30', '60', '240', '720', '1D', '1W', '1M'
 */
using CodexBarResolution = std::variant<, "1", "5", "15", "30", "60", "240", "720", "1D", "1W", "1M">;

/**
 * Response structure for Codex getBars API
 */
struct CodexBarsResponse {
    number[]; // Open prices o;
    number[]; // High prices h;
    number[]; // Low prices l;
    number[]; // Close prices c;
    number[]; // Volume v;
    string[]; // Volume as string volume;
};


/**
 * Standardized candle format used in our application
 */
struct CandleData {
    double open;
    double high;
    double low;
    double close;
    double volume;
    double time;
};


/**
 * Fetch candlestick/OHLC data from Codex API's getBars endpoint
 * @param tokenAddress Token address to fetch candles for (defaults to hardcoded value)
 * @param startTimestamp Start timestamp in seconds
 * @param endTimestamp End timestamp in seconds
 * @param resolution Candle resolution (1min, 5min, 15min, etc.)
 * @param networkId Network ID (default: 1399811149 for Solana)
 * @param quoteToken Quote token to use for price calculation (default: token1)
 * @param env Environment variables containing CODEX_API_KEY
 * @returns Processed candle data in our application's format
 */
std::future<std::vector<CandleData>> fetchCodexBars(const std::string& tokenAddress, double startTimestamp, double endTimestamp, CodexBarResolution = "1" resolution, number = 1399811149 networkId, string = "token1" quoteToken);:${networkId || 1399811149}`;

  // Calculate time interval based on resolution
  let timeInterval: number;
  switch (resolution) {
    case "1D":
      timeInterval = 86400;
      break; // 1 day in seconds
    case "1W":
      timeInterval = 604800;
      break; // 1 week in seconds
    case "1M":
      timeInterval = 2592000;
      break; // 30 days in seconds (approximate)
    default:
      // For minute-based resolutions, convert to seconds
      timeInterval = parseInt(resolution) * 60;
  }

  // If the time range is less than 1000 intervals, fetch all in one request
  if (Math.floor((endTimestamp - startTimestamp) / timeInterval) <= 1000) {
    return fetchCodexBarsChunk(
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
    const maxChunkSize = 1000 * timeInterval;
    let chunks: CandleData[] = [];
    let currentStart = startTimestamp;

    // Process each time chunk sequentially
    while (currentStart < endTimestamp) {
      const chunkEnd = Math.min(currentStart + maxChunkSize, endTimestamp);
      const barsChunk = await fetchCodexBarsChunk(
        apiUrl,
        symbol,
        currentStart,
        chunkEnd,
        resolution,
        quoteToken,
        timeInterval,
      ).catch((error) => {
        logger.error(
          `Error fetching chunk from ${new Date(currentStart * 1000).toISOString()} to ${new Date(chunkEnd * 1000).toISOString()}:`,
          error,
        );
        return [] as CandleData[]; // Return empty array for failed chunks
      });

      chunks = chunks.concat(barsChunk);
      currentStart = chunkEnd;
    }

    // Sort by time to ensure correct order
    chunks.sort((a, b) => a.time - b.time);

    return chunks;
  }
}

/**
 * Helper ,
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
    const response = (await codexService.send(
      `query GetBars($symbol: String!, $from: Int!, $to: Int!, $resolution: String!) {
        getBars(
          symbol: $symbol
          from: $from
          to: $to
          resolution: $resolution
          quoteToken: token1
        ) {
          o
          h
          l
          c
          v
          volume
          liquidity
          buyVolume
          sellVolume
          transactions
        }
      }`,
      {
        symbol: symbol,
        from: startTimestamp,
        to: endTimestamp,
        resolution: resolution,
      },
    )) as any;

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

    if (!(response as any)?.getBars) {
      logger.error("Invalid response from Codex API:", response);

      return [];
    }
    // const barsData = response.data.data.getBars as CodexBarsResponse;
    const barsData = response.getBars as CodexBarsResponse;

    if (!barsData || !barsData.o || barsData.o.length === 0) {
      return [];
    }

    // Convert response arrays to our candle format
    const candleCount = barsData.o.length;
    const result: CandleData[] = [];

    // Create candles starting from the start timestamp
    let currentTime = startTimestamp;

    for (let i = 0; i < candleCount; i++) {
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
      `Range too wide for resolution: ${resolution}, from ${new Date(startTimestamp * 1000).toISOString()} to ${new Date(endTimestamp * 1000).toISOString()}`,
    );
    throw error;
  }
}

} // namespace elizaos
