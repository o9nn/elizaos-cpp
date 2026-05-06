#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_TASKS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_TASKS_H
#include "core.h"
#include "@elizaos/core.h"
#include "./services/updateTracker.h"

extern std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks;
#endif
