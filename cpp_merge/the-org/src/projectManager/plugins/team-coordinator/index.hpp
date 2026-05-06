#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_INDEX_H
#include "core.hpp"
// External dependency removed
#include "./services/CheckInService.h"
// External dependency removed
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
