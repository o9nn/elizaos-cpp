#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> acquireService(IAgentRuntime runtime, auto serviceType, auto asking, auto retries);

std::future<void> askLlmObject(IAgentRuntime runtime, const std::any& ask, const std::vector<std::string>& requiredFields, auto maxRetries);

} // namespace elizaos
