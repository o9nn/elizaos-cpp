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
#include "utils/files.hpp"
#include "utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Remove unfinished trajectories
 */

struct TrajectoryFile {
    std::optional<std::string> submission;
};

/**
 * Remove unfinished trajectories
 */
std::future<void> removeUnfinished(const std::string& baseDir, bool dryRun = true);

} // namespace elizaos
