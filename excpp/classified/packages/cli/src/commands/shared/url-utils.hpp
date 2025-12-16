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


/**
 * Helper function to determine the agent runtime URL
 */
std::string getAgentRuntimeUrl(OptionValues opts);

/**
 * Helper function to get the agents base API URL
 */
std::string getAgentsBaseUrl(OptionValues opts);

} // namespace elizaos
