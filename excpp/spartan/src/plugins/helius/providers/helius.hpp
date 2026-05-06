#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Provider for CMC latest coins
 *
 * @typedef {import('./Provider').Provider} Provider
 * @typedef {import('./Runtime').IAgentRuntime} IAgentRuntime
 * @typedef {import('./Memory').Memory} Memory
 * @typedef {import('./State').State} State
 * @typedef {import('./Action').Action} Action
 *
 * @type {Provider}
 * @property {string} name - The name of the provider
 * @property {string} description - Description of the provider
 * @property {number} position - The position of the provider
 * @property {Function} get - Asynchronous function to get actions that validate for a given message
 *
 * @param {IAgentRuntime} runtime - The agent runtime
 * @param {Memory} message - The message memory
 * @param {State} state - The state of the agent
 * @returns {Object} Object containing data, values, and text related to actions
 */

class HeliusWebSocket {
public:
    HeliusWebSocket();
    std::future<void> connect();
    std::variant<Promise<number, null>> subscribeToWallet(const std::string& walletAddress);
    std::future<bool> unsubscribeFromWallet(const std::string& walletAddress);
    void startPing();
    void cleanup();
    void disconnect();
};

struct IToken {
    std::string symbol;
    std::string name;
    std::string address;
    double decimals;
    double totalSupply;
    double marketCap;
    double volume24h;
    double price;
    double price24hAgo;
    double priceChange24h;
    double curveProgress;
};

      // Get token data from Helius API

      // Process token data

      // Format text response


} // namespace elizaos
