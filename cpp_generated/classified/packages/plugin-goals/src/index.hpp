#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-GOALS_SRC_INDEX_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "./apis.h"
using routes = _default;
#include "./actions/cancelGoal.h"
#include "./actions/completeGoal.h"
#include "./actions/confirmGoal.h"
#include "./actions/createGoal.h"
#include "./actions/updateGoal.h"
#include "./providers/goals.h"
#include "./services/goalService.h"
#include "./schema.h"
#include "./__tests__/e2e.h"
using e2eTestSuites = testSuites;

extern std::shared_ptr<Plugin> GoalsPlugin;
any getGoalsService(std::shared_ptr<IAgentRuntime> runtime);

#endif
