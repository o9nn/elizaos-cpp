#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Create a custom TEE Client to make calls to the TEE through the Dstack SDK.

/**
 * Define the configuration schema for the plugin with the following properties:
 *
 * @param {string} WALLET_SECRET_SALT - The secret salt for the wallet (min length of 1, optional)
 * @returns {object} - The configured schema object
 */

class StarterService {
public:
    StarterService();
    static void start(IAgentRuntime runtime);
    static void stop(IAgentRuntime runtime);

private:
    TappdClient teeClient_;
    std::string secretSalt_;
};

      // Set all environment variables at once
  // Enable this service to run when TEE mode is enabled
    /* StarterService */


} // namespace elizaos
