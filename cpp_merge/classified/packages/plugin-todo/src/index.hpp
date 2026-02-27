#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./apis.h"
#include "./actions/cancelTodo.h"
#include "./actions/completeTodo.h"
#include "./actions/confirmTodo.h"
#include "./actions/createTodo.h"
#include "./actions/updateTodo.h"
#include "./providers/todos.h"
#include "./services/reminderService.h"
#include "./services/todoService.h"
#include "./schema.h"
#include "./__tests__/e2e.h"
using e2eTestSuites = testSuites;

extern std::shared_ptr<Plugin> TodoPlugin;
#endif
