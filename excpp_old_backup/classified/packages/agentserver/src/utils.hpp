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
 * Expands tilde path to current working directory
 */
std::string expandTildePath(const std::string& filepath);

/**
 * Resolves the PGlite directory with proper fallbacks
 */
std::string resolvePgliteDir(std::optional<std::string> dir, std::optional<std::string> fallbackDir);

} // namespace elizaos
