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
 * Inspector server for viewing agent trajectories
 * Converted from sweagent/inspector/server.py
 */

struct TrajectoryStep {
    std: thought;
    std: action;
    std: response;
    std: observation;
};

struct ModelStats {
    std::optional<double> instanceCost;
    std::optional<double> totalCost;
    std::optional<double> instanceInputTokens;
    std::optional<double> instanceOutputTokens;
    std::optional<double> instanceCallCount;
};

struct TrajectoryContent {
    std::vector<TrajectoryStep> trajectory;
    std::optional<std:> exitStatus;
    std::optional<std:> submission;
    std::optional<ModelStats> modelStats;
    std::optional<std:> environment;
};

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
std::optional<TrajectoryContent> getTrajectory(const std:& filePath);

/**
 * Create and start the inspector server
 */
void startInspectorServer(std::optional<std:> options);

/**
 * CLI entry point
 */


} // namespace elizaos
