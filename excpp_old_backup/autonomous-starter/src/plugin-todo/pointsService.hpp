#include "elizaos/core.hpp"
#include "services/componentService.hpp"
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



// Define the structure for the points component data
struct UserPointsData {
    double currentPoints;
    std::optional<std::string> lastPointUpdateReason;
    std::optional<{ timestamp: number; points: number; reason: string }[]; // Optional history> history;

// Define the component structure

/**
 * Calculates points based on task type and completion status.
 * Needs refinement based on specific point values.
 */
double calculatePoints(Task task, "onTime" | "late" | "daily" | "streakBonus" completionStatus);

/**
 * Retrieves the current points for a given entity.
 * Creates the component if it doesn't exist.
 */
std::future<double> getPoints(IAgentRuntime runtime, UUID entityId, UUID roomId, UUID worldId);

/**
 * Adds points to a user's total and updates the component.
 */
std::future<bool> addPoints(IAgentRuntime runtime, UUID entityId, double pointsToAdd, const std::string& reason, UUID roomId, UUID worldId);

} // namespace elizaos
