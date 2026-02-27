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
#include "registry.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Diff state tool
 * Tracks git diff state in a JSON file
 * Converted from tools/diff_state/
 */

struct State {
    std::optional<std::string> diff;
};

State loadState();

void saveState(State state);

void updateDiffState();

void clearDiff();

std::string getDiff();

// CLI if run directly

  // Default action


} // namespace elizaos
