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



// Proxy RPC requests to Alchemy to keep API key server-side
// This prevents the Alchemy API key from being exposed in the browser

std::future<void> POST(NextRequest request);

} // namespace elizaos
