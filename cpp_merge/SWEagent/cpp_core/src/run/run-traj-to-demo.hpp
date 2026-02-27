#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "utils/log.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Convert a trajectory file to a yaml file for editing of demos.
 * You can then load the yaml file with run_replay to replay the actions in an environment
 * to get environment output.
 */

/**
 * Save demo data as a yaml file with proper header
 */
void saveDemo(const std:& data, const std:& file, const std:& trajPath);

/**
 * Convert trajectory to action demo
 */
void convertTrajToActionDemo(const std:& trajPath, const std:& outputFile, bool includeUser = false);

/**
 * Main std::function for traj-to-demo conversion
 */
void trajToDemo(const std:& trajPath, std: outputDir = "./demos", std: suffix = "", bool overwrite = false, bool includeUser = false);

} // namespace elizaos
