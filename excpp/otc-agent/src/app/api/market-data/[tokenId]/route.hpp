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
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Check if we're in local development mode (no external API calls needed)
bool isLocalDevelopment(const std::string& chain, const std::string& contractAddress);

std::future<void> GET(NextRequest request);

} // namespace elizaos
