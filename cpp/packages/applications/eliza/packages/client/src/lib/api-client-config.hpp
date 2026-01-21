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
#include "elizaos/api-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



ApiClientConfig createApiClientConfig();

// Singleton instance

ElizaClient createElizaClient();

ElizaClient getElizaClient();

// Function to reset the singleton (useful for API key changes)
void resetElizaClient();

void updateApiClientApiKey(const std::string& newApiKey);

} // namespace elizaos
