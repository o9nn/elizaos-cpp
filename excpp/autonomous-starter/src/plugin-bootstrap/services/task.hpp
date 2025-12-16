#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// registered to runtime through plugin

/**
 * TaskService class representing a service that schedules and executes tasks.
 * @extends Service
 * @property {NodeJS.Timeout|null} timer - Timer for executing tasks
 * @property {number} TICK_INTERVAL - Interval in milliseconds to check for tasks
 * @property {ServiceTypeName} serviceType - Service type of TASK
 * @property {string} capabilityDescription - Description of the service's capability
 * @static
 * @method start - Static method to start the TaskService
 * @method createTestTasks - Method to create test tasks
 * @method startTimer - Private method to start the timer for checking tasks
 * @method validateTasks - Private method to validate tasks
 * @method checkTasks - Private method to check tasks and execute them
 * @method executeTask - Private method to execute a task
 * @static
 * @method stop - Static method to stop the TaskService
 * @method stop - Method to stop the TaskService
 */
/**
 * Start the TaskService with the given runtime.
 * @param {IAgentRuntime} runtime - The runtime for the TaskService.
 */
class TaskService {
public:
    std::future<TaskService> start(IAgentRuntime runtime);
    void createTestTasks();
    void startTimer();
    std::future<std::vector<Task>> validateTasks(const std::vector<Task>& tasks);
    void checkTasks();
    void executeTask(Task task);
    void stop(IAgentRuntime runtime);
    void stop();
};


} // namespace elizaos
