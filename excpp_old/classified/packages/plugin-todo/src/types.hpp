#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


// Extend the core service types with todo service
  struct ServiceTypeRegistry {
    'TODO' TODO;
    'TODO_REMINDER' TODO_REMINDER;
};

// Export service type constant

/**
 * Type for todo metadata
 */
using TodoMetadata = std::variant<Record<string, string, double, bool, nullptr, undefined>>;

/**
 * Todo-specific types
 */
struct TodoData {
    UUID id;
    UUID agentId;
    UUID worldId;
    UUID roomId;
    UUID entityId;
    std::string name;
    std::optional<std::string> description;
    std::variant<'one-off', 'daily'> type;
    double priority;
    bool isUrgent;
    bool isCompleted;
    std::optional<Date> dueDate;
    std::optional<Date> completedAt;
    Date createdAt;
    Date updatedAt;
    TodoMetadata metadata;
    std::optional<std::vector<std::string>> tags;
};

struct CreateTodoInput {
    UUID agentId;
    UUID worldId;
    UUID roomId;
    UUID entityId;
    std::string name;
    std::optional<std::string> description;
    std::variant<'one-off', 'daily'> type;
    std::optional<double> priority;
    std::optional<bool> isUrgent;
    std::optional<Date> dueDate;
    std::optional<TodoMetadata> metadata;
    std::optional<std::vector<std::string>> tags;
};

struct UpdateTodoInput {
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::variant<'one-off', 'daily'>> type;
    std::optional<double> priority;
    std::optional<bool> isUrgent;
    std::optional<bool> isCompleted;
    std::optional<Date> dueDate;
    std::optional<Date> completedAt;
    std::optional<TodoMetadata> metadata;
};


} // namespace elizaos
