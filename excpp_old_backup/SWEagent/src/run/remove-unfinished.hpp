#include ".utils/files.hpp"
#include ".utils/log.hpp"
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
 * Remove unfinished trajectories
 */

struct TrajectoryFile {
    std::optional<{> info;
    std::optional<string | null> submission;
};

/**
 * Remove unfinished trajectories
 */
std::future<void> removeUnfinished(const std::string& baseDir, boolean = true dryRun);

} // namespace elizaos
