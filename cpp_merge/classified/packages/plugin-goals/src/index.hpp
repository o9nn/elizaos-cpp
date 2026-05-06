#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_INDEX_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_GOALS_SRC_INDEX_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "./apis.h"
// Using alias removed (invalid transpilation)
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
