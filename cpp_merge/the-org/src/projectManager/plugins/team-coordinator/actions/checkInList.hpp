#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_CHECKINLIST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_THE-ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM-COORDINATOR_ACTIONS_CHECKINLIST_H
#include "core.h"
#include "@elizaos/core.h"
#include "../../../types.h"

std::shared_ptr<Promise<array<std::shared_ptr<CheckInSchedule>>>> fetchCheckInSchedules(std::shared_ptr<IAgentRuntime> runtime);

string formatSchedule(std::shared_ptr<CheckInSchedule> schedule);

extern std::shared_ptr<Action> listCheckInSchedules;
#endif
