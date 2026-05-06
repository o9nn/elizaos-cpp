#include "constants.hpp"
#include "elizaos/core.hpp"
#include "utils.hpp"
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



// emulate settings from 0.x

/*
class BaseCachedProvider {
    private cache: NodeCache;

    constructor(
        private cacheManager: ICacheManager,
        private cacheKey,
        ttl?: number
    ) {
        this.runtime = runtime
        this.cache = new NodeCache({ stdTTL: ttl || 300 });
    }

        // get memory cache first

            // set to memory cache

        // Set in-memory cache

        // Write to file-based cache
*/

// extends BaseCachedProvider
// really more like a service
class BirdeyeProvider {
    private symbolMap: Record<string, string>;
    private maxRetries: number;

    constructor(
        runtime,
        //cacheManager: ICacheManager,
        symbolMap?: Record<string, string>,
        maxRetries?: number
    ) {
        // super(cacheManager, "birdeye/data")
        this.runtime = runtime
        this.symbolMap = symbolMap || DEFAULT_SUPPORTED_SYMBOLS;
        this.maxRetries = maxRetries || DEFAULT_MAX_RETRIES;
    }

    /*
     * COMMON FETCH FUNCTIONS
     */

        // allow the user to override the chain

                // If the response already has data and success fields, return it
                // Otherwise wrap the response in the expected format
                    // failed after all
                await waitFor(RETRY_DELAY_MS);

    /*
     * DEFI FETCH FUNCTIONS
     */

    // Get a list of all supported networks.

    // Get price update of a token.

    // Get price updates of multiple tokens in a single API call. Maximum 100 tokens

    // Get price updates of multiple tokens in a single API call. Maximum 100 tokens

    // Get historical price line chart of a token.

    // Get historical price by unix timestamp

    // Get list of trades of a certain token.

    // Get list of trades of a certain pair or market.

    // Get list of trades of a token with time bound option.

    // Get list of trades of a certain pair or market with time bound option.

    // Get OHLCV price of a token.

    // Get OHLCV price of a pair.

    // Get OHLCV price of a base-quote pair.

    // Get price and volume of a token.

    // Get price and volume updates of maximum 50 tokens

    /*
     * TOKEN FETCH FUNCTIONS
     */

    // Get token list of any supported chains.

    // Get token security of any supported chains.

    // Get overview of a token.

    // Get creation info of token

    // Retrieve a dynamic and up-to-date list of trending tokens based on specified sorting criteria.

    // This endpoint facilitates the retrieval of a list of tokens on a specified blockchain network. This upgraded version is exclusive to business and enterprise packages. By simply including the header for the requested blockchain without any query parameters, business and enterprise users can get the full list of tokens on the specified blockchain in the URL returned in the response. This removes the need for the limit response of the previous version and reduces the workload of making multiple calls.

    // Get newly listed tokens of any supported chains.

    // Get top traders of given token.

    // The API provides detailed information about the markets for a specific cryptocurrency token on a specified blockchain. Users can retrieve data for one or multiple markets related to a single token. This endpoint requires the specification of a token address and the blockchain to filter results. Additionally, it supports optional query parameters such as offset, limit, and required sorting by liquidity or sort type (ascending or descending) to refine the output.

    // Get metadata of single token

    // Get metadata of multiple tokens

    // Get market data of single token

    // Get trade data of single token

    // Get trade data of multiple tokens

    // Get top holder list of the given token

    // Get mint/burn transaction list of the given token. Only support solana currently

    /*
     * WALLET FETCH FUNCTIONS
     */

    /**
     * @deprecated This endpoint will be decommissioned on Feb 1st, 2025.
     */

    /**
     * @deprecated This endpoint will be decommissioned on Feb 1st, 2025.
     */

    /*
     * TRADER FETCH FUNCTIONS
     */

    // The API provides detailed information top gainers/losers

    // Get list of trades of a trader with time bound option.

    /*
     * PAIR FETCH FUNCTIONS
     */

    // Get overview of multiple pairs

    /*
     * SEARCH FETCH FUNCTIONS
     */

} // namespace elizaos
