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
 * Convert a trajectory file to a yaml file for editing of demos.
 * You can then load the yaml file with run_replay to replay the actions in an environment
 * to get environment output.
 */

/**
 * Save demo data as a yaml file with proper header
 */
void saveDemo(unknown data, const std::string& file, const std::string& trajPath);

/**
 * Convert trajectory to action demo
 */
void convertTrajToActionDemo(const std::string& trajPath, const std::string& outputFile, boolean = false includeUser);

/**
 * Main function for traj-to-demo conversion
 */
void trajToDemo(const std::string& trajPath, string = './demos' outputDir, string = '' suffix, boolean = false overwrite, boolean = false includeUser);

} // namespace elizaos
