#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Clear all tokens from the database (dev utility)
 * POST /api/tokens/clear
 */
std::future<void> POST();

} // namespace elizaos
