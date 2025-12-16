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
 * Defines the contract for a Task Worker, which is responsible for executing a specific type of task.
 * Task workers are registered with the `AgentRuntime` and are invoked when a `Task` of their designated `name` needs processing.
 * This pattern allows for modular and extensible background task processing.
 */
struct TaskWorker {
    std::string name;
    ( execute;
    IAgentRuntime runtime;
    { [key: string]: unknown } options;

/**
 * Defines metadata associated with a `Task`.
 * This can include scheduling information like `updateInterval` or UI-related details
 * for presenting task options to a user.
 * The `[key: string]: unknown;` allows for additional, unspecified metadata fields.
 */
using TaskMetadata = {
  /** Optional. If the task is recurring, this specifies the interval in milliseconds between updates or executions. */
  /** Optional. Describes options or parameters that can be configured for this task, often for UI presentation. */
  /** Allows for other dynamic metadata properties related to the task. */

/**
 * Represents a task to be performed, often in the background or at a later time.
 * Tasks are managed by the `AgentRuntime` and processed by registered `TaskWorker`s.
 * They can be associated with a room, world, and tagged for categorization and retrieval.
 * The `IDatabaseAdapter` handles persistence of task data.
 */
struct Task {
    std::optional<UUID> id;
    std::string name;
    std::optional<double> updatedAt;
    std::optional<TaskMetadata> metadata;
    std::string description;
    std::optional<UUID> roomId;
    std::optional<UUID> worldId;
    std::optional<UUID> entityId;
    std::vector<std::string> tags;
};


} // namespace elizaos
