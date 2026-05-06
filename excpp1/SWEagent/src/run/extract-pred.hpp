#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * If for some reason the .pred file isn't saved, we can extract it from the .traj file
 */

/**
 * Extract prediction from trajectory file
 */
void extractPred(const std::string& trajPath);

} // namespace elizaos
