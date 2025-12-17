#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Helper function to determine the agent runtime URL
 */
std::string getAgentRuntimeUrl(OptionValues opts);

/**
 * Helper function to get the agents base API URL
 */
std::string getAgentsBaseUrl(OptionValues opts);

} // namespace elizaos
