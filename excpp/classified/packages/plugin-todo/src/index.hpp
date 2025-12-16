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

import type { Plugin, IAgentRuntime } from '@elizaos/core';

;

// Import actions
;
;
;
;
;

// Import providers
;

// Import services
;
;

// Import schema
;

// Import table schemas for registration

// Import tests
;

/**
 * The TodoPlugin provides task management functionality with daily recurring and one-off tasks,
 * including creating, completing, updating, and deleting tasks, as well as a point system for
 * task completion.
 */
const TodoPlugin: Plugin = {
  name: 'todo',
  description: 'Provides task management functionality with daily recurring and one-off tasks.',
  providers: [todosProvider],
  dependencies: ['@elizaos/plugin-sql'],
  testDependencies: ['@elizaos/plugin-sql'],
  actions: [
    createTodoAction,
    completeTodoAction,
    confirmTodoAction,
    updateTodoAction,
    cancelTodoAction,
  ],
  services: [TodoService, TodoReminderService],
  routes,
  schema: todoSchemaExport,
  tests: e2eTestSuites,
  init: async (_config: Record<string, string>, _runtime: IAgentRuntime) => {
    // Plugin initialization - services are automatically started by the runtime
  },
};

default TodoPlugin;

// Export services for external use
{ TodoService } from './services/todoService';
{ TodoReminderService } from './services/reminderService';

} // namespace elizaos
