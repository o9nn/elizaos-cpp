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
void saveDemo(const std::any& data, const std::string& file, const std::string& trajPath);

/**
 * Convert trajectory to action demo
 */
void convertTrajToActionDemo(const std::string& trajPath, const std::string& outputFile, bool includeUser = false);

/**
 * Main function for traj-to-demo conversion
 */
void trajToDemo(const std::string& trajPath, std::string outputDir = "./demos", std::string suffix = "", bool overwrite = false, bool includeUser = false);

} // namespace elizaos
