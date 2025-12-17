#include ".constants.hpp"
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



// Define the ProviderResult interface if not already imported
/**
 * Represents the result returned by a provider.
 * @typedef {Object} ProviderResult
 * @property {any} [data] - The data associated with the result.
 * @property {Record<string, string>} [values] - The values stored in key-value pairs.
 * @property {string} [text] - The text content of the result.
 */
struct ProviderResult {
    std::optional<std::any> data;
    std::optional<std::unordered_map<std::string, std::string>> values;
    std::optional<std::string> text;
};

/**
 * Wallet provider for Solana.
 * @param {IAgentRuntime} runtime - The agent runtime.
 * @param {Memory} _message - The memory message.
 * @param {State} [state] - Optional state parameter.
 * @returns {Promise<ProviderResult>} The result of the wallet provider.
 */
  // it's not slow we always have this data
  // but we don't always need this data, let's free up the context
      //console.log('portfolioCache', portfolioCache)

      // hard coding service name, ugh
      // why wouldn't this exist? it's in the same plugin...

      // Values that can be injected into templates

      // Add token balances to values

      // Add market prices to values

      // Format the text output

      // Token Balances

      // Market Prices


} // namespace elizaos
