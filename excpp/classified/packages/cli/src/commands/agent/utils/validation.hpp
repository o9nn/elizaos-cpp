#include "..shared.hpp"
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



// Zod schemas for validation

/**
 * Asynchronously fetches a list of basic agent information from the server.
 */

/**
 * Resolves the ID of an agent based on the provided name, ID, or index.
 */
std::future<std::string> resolveAgentId(const std::string& idOrNameOrIndex, OptionValues opts);

} // namespace elizaos
