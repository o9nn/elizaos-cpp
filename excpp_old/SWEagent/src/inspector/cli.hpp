#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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
};

struct Trajectory {
    std::vector<TrajectoryStep> trajectory;
    std::optional<std::string> exitStatus;
    std::optional<std::string> submission;
    std::optional<double> instanceCost;
    std::optional<double> instanceInputTokens;
    std::optional<double> instanceOutputTokens;
    std::optional<double> instanceCallCount;
    std::optional<std::string> environment;
};

/**
 * Load and parse a trajectory file
 */
std::optional<Trajectory> loadTrajectory(const std::string& filePath);

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

      // Display all steps

    // Sort files

// Parse arguments

// Show help if no command

} // namespace elizaos
