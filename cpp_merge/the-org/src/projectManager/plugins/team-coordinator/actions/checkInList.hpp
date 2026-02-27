#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_CHECKINLIST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_THE_ORG_SRC_PROJECTMANAGER_PLUGINS_TEAM_COORDINATOR_ACTIONS_CHECKINLIST_H
#include "core.hpp"
// External dependency removed
#include "../../../types.h"

std::shared_ptr<Promise<array<std::shared_ptr<CheckInSchedule>>>> fetchCheckInSchedules(std::shared_ptr<IAgentRuntime> runtime);

string formatSchedule(std::shared_ptr<CheckInSchedule> schedule);

extern std::shared_ptr<Action> listCheckInSchedules;
#endif
