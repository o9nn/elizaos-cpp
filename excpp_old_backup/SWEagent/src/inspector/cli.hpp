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
 * CLI inspector for viewing agent trajectories
 * Converted from inspector_cli.py
 */

struct TrajectoryStep {
    std::optional<std::string> thought;
    std::optional<std::string> action;
    std::optional<std::string> response;
    std::optional<std::string> observation;
    std::optional<std::vector<{ role: string; content: string }>> messages;

struct Trajectory {
    std::optional<std::vector<{ role: string; content: string }>> history;

/**
 * Load and parse a trajectory file
 */

/**
 * Format a trajectory step for display
 */
std::string formatStep(TrajectoryStep step, double index);

/**
 * Display trajectory summary
 */
void displaySummary(Trajectory trajectory);

/**
 * Interactive mode for stepping through trajectory
 */
std::future<void> interactiveMode(Trajectory trajectory);

/**
 * Main CLI program
 */

      await interactiveMode(trajectory);
      // Display all steps

    // Sort files

// Parse arguments

// Show help if no command

} // namespace elizaos
