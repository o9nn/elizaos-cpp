#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "./services/CheckInService.h"
#include "@elizaos/core.h"
#include "./actions/checkInList.h"
#include "./services/updateTracker.h"
#include "./actions/checkInCreate.h"
#include "./actions/reportGenerate.h"
#include "./actions/teamMemberUpdate.h"
#include "./actions/teamMemberAdd.h"
#include "./actions/teamMembersList.h"
#include "./actions/updateFormat.h"
#include "./tasks.h"

extern std::shared_ptr<Plugin> teamCoordinatorPlugin;
any initialize(std::shared_ptr<IAgentRuntime> runtime);

#endif
