#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_REPORTGENERATE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_REPORTGENERATE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../../../types.h"

std::shared_ptr<Promise<string>> generateTeamReport(std::shared_ptr<IAgentRuntime> runtime, string standupType, string roomId = undefined);

extern std::shared_ptr<Action> generateReport;
#endif
