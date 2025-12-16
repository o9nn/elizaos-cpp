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



struct EntityWalletResult {
    std::string walletAddress;
    std::optional<std::unordered_map<std::string, std::any>> metadata;
};

struct EntityWalletError {
    ActionResult result;
};

using EntityWalletResponse = std::variant<EntityWalletResult, EntityWalletError>;

/**
 * Retrieves entity wallet information from runtime and validates it exists.
 * Returns either the wallet address on success, or a complete ActionResult on failure.
 */
std::future<EntityWalletResponse> getEntityWallet(IAgentRuntime runtime, Memory message, const std::string& actionName, std::optional<HandlerCallback> callback);

} // namespace elizaos
