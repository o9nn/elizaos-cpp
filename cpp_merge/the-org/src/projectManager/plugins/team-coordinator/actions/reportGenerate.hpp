#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_REPORTGENERATE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_REPORTGENERATE_H
#include "core.hpp"
// External dependency removed
#include "../../../types.h"

std::shared_ptr<Promise<string>> generateTeamReport(std::shared_ptr<IAgentRuntime> runtime, string standupType, string roomId = undefined);

extern std::shared_ptr<Action> generateReport;
#endif
