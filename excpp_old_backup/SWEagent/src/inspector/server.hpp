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
 * Inspector server for viewing agent trajectories
 * Converted from sweagent/inspector/server.py
 */

struct TrajectoryStep {
    std::string thought;
    std::string action;
    std::string response;
    std::string observation;
    std::optional<std::vector<{ role: string; content: string }>> messages;

struct ModelStats {
    std::optional<double> instanceCost;
    std::optional<double> totalCost;
    std::optional<double> instanceInputTokens;
    std::optional<double> instanceOutputTokens;
    std::optional<double> instanceCallCount;
};

struct TrajectoryContent {
    std::vector<{ role: string; content: string }> history;

/**
 * Add problem statement to the trajectory
 * The problem statement is the first 'user' message in the history
 */
TrajectoryContent addProblemStatement(TrajectoryContent content);

/**
 * Append exit status to the trajectory
 */
TrajectoryContent appendExit(TrajectoryContent content);

/**
 * Add model statistics to the trajectory
 */
TrajectoryContent addModelStats(TrajectoryContent content);

/**
 * Parse and augment trajectory file
 */

/**
 * Create and start the inspector server
 */
void startInspectorServer(std::optional<{ port: number; trajectoryDir: string; staticDir: string }> options);

/**
 * CLI entry point
 */


} // namespace elizaos
