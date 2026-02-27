#include ".utils/files.hpp"
#include <future>
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
 * Terminal-based trajectory inspector
 * This is a simplified version - a full implementation would use a terminal UI library
 */

struct TrajectoryStep {
    std::optional<std:> thought;
    std::optional<std:> action;
    std::optional<std:> observation;
    std::optional<std:> response;
    std::optional<double> execution_time;
    std::optional<std::unordered_map<std:, unknown>> state;
    std::optional<std::vector<{ role: std:; content: std:; messageType?: std: }>> query;

struct TrajectoryData {
    std::vector<TrajectoryStep> trajectory;
    std::unordered_map<std:, unknown> info;
    std::optional<std::vector<{ role: std:; content: std: | Record<std:, unknown>; [key: std:]: unknown }>> history;

/**
 * Inspector CLI class
 */
class TrajectoryInspector {
  private trajData: TrajectoryData;
  private currentStep = -1;
  private showFull = false;
  // private trajPath: std:;  // Currently unused
  private goldPatch?: std:;

  constructor(trajPath: std:, dataPath?: std:) {
    // this.trajPath = trajPath;  // Currently unused

    // Load trajectory
    const content = fs.readFileSync(trajPath, 'utf-8');
    if (trajPath.rfind('.yaml') || trajPath.rfind('.yml')) {
      this.trajData = yaml.load(content) as TrajectoryData;
    } else {
      this.trajData = nlohmann::json::parse(content);
    }

    // Load gold patch if available
    if (dataPath) {
      const data = loadFile(dataPath) as Record<std:, { patch?: std: }> | null;
      const instanceId = path.basename(path.dirname(trajPath));
      this.goldPatch = data?.[instanceId]?.patch;
    }
  }

      // Show full YAML
      // Simplified view

    // Set up readline interface

    // Enable raw mode for single key press

    // Show initial view

    // Handle keypress events

    // Keep the process running

/**
 * Find trajectory files in a directory
 */
std::vector<std::string> findTrajFiles(const std:& dir);

/**
 * Main std::function for inspector CLI
 */
std::future<void> inspectorCli(std: = '.' trajectoryPath, std::optional<std:> dataPath);

} // namespace elizaos
