#include "elizaos/core.hpp"
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



/**
 * Interface representing a transaction history entry.
 * @property {string} txHash - The hash of the transaction.
 * @property {Date} blockTime - The time when the transaction occurred.
 * @property {any} data - Additional data related to the transaction.
 */
struct TransactionHistory {
    std::string txHash;
    Date blockTime;
    std::any data;
};

/**
 * Interface representing a Portfolio object.
 * @typedef {Object} Portfolio
 * @property {string} key - The key associated with the portfolio.
 * @property {any} data - The data contained in the portfolio.
 */
struct Portfolio {
    std::string key;
    std::any data;
};

/**
 * Interface representing content with sentiment analysis data.
 * @extends Content
 * @interface
 */

/**
 * Template for analyzing tweets related to the cryptocurrency market.
 *
 * The template prompts the user to write a summary of the tweets and analyze the tokens present in the tweet.
 * It requests information on whether the sentiment towards the tokens is positive or negative.
 *
 * To analyze the given tweets, the user needs to strictly return a JSON object with the following structure:
 * {
 *     "text": "the summary of what has happened in those tweets, with a max length of 200 characters",
 *     "occuringTokens": [
 *         {
 *             "token": "the token symbol, like: ETH, SOL, BTC etc.",
 *             "sentiment": "positive is between 1 and 100 and negative is from -1 to -100",
 *             "reason": "a short sentence explaining the reason for this sentiment score"
 *         }
 *     ]
 * }
 */

/**
 * Generates a bulletpoint list from an array of strings.
 *
 * @param {string[]} array - The array of strings to create the list from.
 * @returns {string} The bulletpoint list as a single string with new lines separating each item.
 */
std::string makeBulletpointList(const std::vector<std::string>& array);

      // First get data from Birdeye

      //console.log('birdeyeData', resp)

      //console.log('birdeyeData', birdeyeData)

      //console.log('intel/be/syncWalletHistory - birdeyeData', birdeyeData)

      // Convert Birdeye data to our transaction format

      //console.log('intel/be/syncWalletHistory - transactions', transactions)

      // Then try to get cached transactions
        // FIXME: scope to solana and wallet
          // Add cached transactions that don't exist in Birdeye data
        // If cache fails, continue with just Birdeye data

      // ensure all dates are of type Date for sort

      // Sort transactions by blockTime descending (newest first)

      // Try to update cache, but don't fail if it doesn't work

      // Return empty array if everything fails

    /** Get entire portfolio */

    //console.log('portfolio data', data)

    // maybe let solana or birdeye plugin handle this

      // Get existing tokens

      /** Fetch top 100 in batches of 20 (which is the limit) */
        //console.log('trending', resp)

        // Add extra delay
      //console.log('trending tokens', tokens)


} // namespace elizaos
