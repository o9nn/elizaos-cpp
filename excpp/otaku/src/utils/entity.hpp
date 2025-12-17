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
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct EntityWalletResult {
    std::string walletAddress;
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
