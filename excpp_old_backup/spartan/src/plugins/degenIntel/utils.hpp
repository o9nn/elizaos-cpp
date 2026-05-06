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



std::future<void> acquireService(IAgentRuntime runtime, auto serviceType, auto asking = '', auto retries = 10);

std::future<void> askLlmObject(IAgentRuntime runtime, Object ask, const std::vector<std::string>& requiredFields, auto maxRetries = 3);

} // namespace elizaos
