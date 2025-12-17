#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "dbCompatibility.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Task service that provides database-compatible task operations
 */
class TaskService {
public:
    TaskService();
    std::future<std::vector<Task>> getTasks(std::optional<std::any> params);
    std::variant<Promise<UUID, null>> createTask(std::optional<std::any> params);
    std::future<bool> updateTask(UUID taskId, std::optional<std::any> updates);
    Task normalizeTask(Task task);
    std::future<std::vector<Task>> searchTasksByName(const std::string& name, std::optional<std::any> params);
};

/**
 * Create a task service instance for the runtime
 */
TaskService createTaskService(IAgentRuntime runtime); 
} // namespace elizaos
