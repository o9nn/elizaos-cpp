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
#include ".schema.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Core todo data structure
 */
struct TodoData {
    UUID id;
    UUID agentId;
    UUID worldId;
    UUID roomId;
    UUID entityId;
    std::string name;
    std::optional<std::string> description;
    std::string type;
    std::optional<std::optional<double>> priority;
    bool isUrgent;
    bool isCompleted;
    std::optional<std::optional<Date>> dueDate;
    std::optional<std::optional<Date>> completedAt;
    Date createdAt;
    Date updatedAt;
    TodoMetadata metadata;
    std::optional<std::vector<std::string>> tags;
};

/**
 * Manages todo data and database operations
 */
class TodoDataManager {
public:
    TodoDataManager(IAgentRuntime runtime);
    std::future<UUID> createTodo(std::optional<std::any> data);
    std::variant<Promise<TodoData, null>> getTodo(UUID todoId);
    std::future<std::vector<TodoData>> getTodos(std::optional<std::any> filters);
    std::future<bool> updateTodo(UUID todoId, std::optional<std::any> updates);
    std::future<bool> deleteTodo(UUID todoId);
    std::future<bool> addTags(UUID todoId, const std::vector<std::string>& tags);
    std::future<bool> removeTags(UUID todoId, const std::vector<std::string>& tags);
    std::future<std::vector<TodoData>> getOverdueTodos(std::optional<std::any> filters);
    std::future<double> resetDailyTodos(std::optional<std::any> filters);

private:
    IAgentRuntime runtime_;
};

/**
 * Create a new TodoService instance
 */
TodoDataManager createTodoDataService(IAgentRuntime runtime);

/**
 * Service wrapper for database operations
 */
class TodoService {
public:
    std::future<void> stop();
    static std::future<TodoService> start(IAgentRuntime runtime);
    std::optional<TodoDataManager> getDataService();
    std::variant<Promise<UUID, null>> createTodo(std::optional<std::any> params);
    std::future<std::vector<TodoData>> getTodos(std::optional<std::any> filters);
    std::future<bool> updateTodo(UUID todoId, std::optional<std::any> updates);
    std::future<bool> deleteTodo(UUID todoId);
    std::variant<Promise<TodoData, null>> getTodo(UUID todoId);
    std::future<bool> addTags(UUID todoId, const std::vector<std::string>& tags);
    std::future<bool> removeTags(UUID todoId, const std::vector<std::string>& tags);
    std::future<std::vector<TodoData>> getOverdueTodos(std::optional<std::any> filters);
    std::future<double> resetDailyTodos(std::optional<std::any> filters);
};


} // namespace elizaos
