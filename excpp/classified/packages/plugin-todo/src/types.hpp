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

import type { UUID } from '@elizaos/core';

// Extend the core service types with todo service
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'TODO' TODO;
    'TODO_REMINDER' TODO_REMINDER;
};

}

// Export service type constant
const TodoServiceType = {
  TODO: 'TODO' as const,
  TODO_REMINDER: 'TODO_REMINDER' as const,
} satisfies Partial<import('@elizaos/core').ServiceTypeRegistry>;

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
    'one-off' | 'daily' type;
    number; // 1-5, with 1 being highest priority priority;
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
    'one-off' | 'daily' type;
    std::optional<double> priority;
    std::optional<bool> isUrgent;
    std::optional<Date> dueDate;
    std::optional<TodoMetadata> metadata;
    std::optional<std::vector<std::string>> tags;
};


struct UpdateTodoInput {
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<'one-off' | 'daily'> type;
    std::optional<double> priority;
    std::optional<bool> isUrgent;
    std::optional<bool> isCompleted;
    std::optional<Date> dueDate;
    std::optional<Date> completedAt;
    std::optional<TodoMetadata> metadata;
};


} // namespace elizaos
