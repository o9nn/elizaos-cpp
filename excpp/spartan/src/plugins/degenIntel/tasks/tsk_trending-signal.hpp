#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Template for generating a crypto buy signal based on sentiment analysis and trending tokens.
 *
 * Sentiment analysis:
 * {{sentiment}}
 *
 * Trending tokens:
 * {{trending_tokens}}
 *
 * Only return the following JSON:
 * {
 *     recommended_buy: "the symbol of the token for example DEGENAI",
 *     recommend_buy_address: "the address of the token to purchase, for example: 2sCUCJdVkmyXp4dT8sFaA9LKgSMK4yDPi9zLHiwXpump",
 *     reason: "the reason why you think this is a good buy, and why you chose the specific amount",
 *     buy_amount: "number, for example: 0.1"
 * }
 */

/**
 * Interface representing the output of a buy signal.
 * @typedef {object} IBuySignalOutput
 * @property {string} recommended_buy - The recommended buy action.
 * @property {string} recommend_buy_address - The recommended buy address.
 * @property {number} marketcap - The marketcap value.
 * @property {string} reason - The reason for the buy recommendation.
 * @property {string} buy_amount - The amount to buy.
 */
struct IBuySignalOutput {
    std::string recommended_buy;
    std::string recommend_buy_address;
    double marketcap;
    std::string reason;
    std::string buy_amount;
};

    std::future<bool> generateSignal();
    void getBalance();


} // namespace elizaos
