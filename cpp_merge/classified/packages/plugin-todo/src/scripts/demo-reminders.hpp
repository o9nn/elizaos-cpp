#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_SCRIPTS_DEMO_REMINDERS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_TODO_SRC_SCRIPTS_DEMO_REMINDERS_H
#include "core.hpp"
// External dependency removed
#include "../services/todoService.h"
#include "../services/reminderService.h"
#include "uuid.hpp"
using uuidv4 = v4;

any setupReminderDemo(std::shared_ptr<IAgentRuntime> runtime);

void monitorReminders(std::shared_ptr<IAgentRuntime> runtime, double duration = 60000);

void cleanupDemo(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<UUID> testUserId);

#endif
