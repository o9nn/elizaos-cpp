#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "__tests__/e2e.hpp"
#include "actions/cancelTodo.hpp"
#include "actions/completeTodo.hpp"
#include "actions/confirmTodo.hpp"
#include "actions/createTodo.hpp"
#include "actions/updateTodo.hpp"
#include "apis.hpp"
#include "providers/todos.hpp"
#include "schema.hpp"
#include "services/reminderService.hpp"
#include "services/todoService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Import actions

// Import providers

// Import services

// Import schema

// Import table schemas for registration

// Import tests

/**
 * The TodoPlugin provides task management functionality with daily recurring and one-off tasks,
 * including creating, completing, updating, and deleting tasks, as well as a point system for
 * task completion.
 */
    // Plugin initialization - services are automatically started by the runtime

// Export services for external use

} // namespace elizaos
