#include ".utils/files.hpp"
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
    std::optional<std::string> thought;
    std::optional<std::string> action;
    std::optional<std::string> observation;
    std::optional<std::string> response;
    std::optional<double> execution_time;
    std::optional<std::unordered_map<std::string, unknown>> state;
    std::optional<std::vector<{ role: string; content: string; messageType?: string }>> query;

struct TrajectoryData {
    std::vector<TrajectoryStep> trajectory;
    std::unordered_map<std::string, unknown> info;
    std::optional<std::vector<{ role: string; content: string | Record<string, unknown>; [key: string]: unknown }>> history;

/**
 * Inspector CLI class
 */
class TrajectoryInspector {
  private trajData: TrajectoryData;
  private currentStep: number = -1;
  private showFull: boolean = false;
  // private trajPath: string;  // Currently unused
  private goldPatch?: string;

  constructor(trajPath: string, dataPath?: string) {
    // this.trajPath = trajPath;  // Currently unused

    // Load trajectory
    const content = fs.readFileSync(trajPath, 'utf-8');
    if (trajPath.endsWith('.yaml') || trajPath.endsWith('.yml')) {
      this.trajData = yaml.load(content) as TrajectoryData;
    } else {
      this.trajData = JSON.parse(content);
    }

    // Load gold patch if available
    if (dataPath) {
      const data = loadFile(dataPath) as Record<string, { patch?: string }> | null;
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
std::vector<std::string> findTrajFiles(const std::string& dir);

/**
 * Main function for inspector CLI
 */
std::future<void> inspectorCli(string = '.' trajectoryPath, std::optional<std::string> dataPath);

} // namespace elizaos
