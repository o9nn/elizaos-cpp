#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "elizaos/core.hpp"

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

    void syncWalletHistory();
    void syncWalletPortfolio();
    void syncWallet();
    std::future<bool> syncTrendingTokens(const std::variant<'solana', 'base'>& chain);


} // namespace elizaos
