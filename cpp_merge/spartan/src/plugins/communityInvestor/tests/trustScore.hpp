#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_TESTS_TRUSTSCORE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_TESTS_TRUSTSCORE_H
#include "core.hpp"
// External dependency removed
using CoreComponent = Component;
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "../service.h"
#include "../types.h"
#include "../types.h"

extern any testUserIdGlobalTrustScore;
extern any testWorldId;
extern std::function<any(any, any, any)> createFullMockComponentForTrustScore;
extern std::function<any(string, double, any, any, any, double)> createRecForTrustScore;
extern array<std::shared_ptr<TestCase>> recencyWeightTests;
extern array<std::shared_ptr<TestCase>> convictionWeightTests;
extern array<std::shared_ptr<TestCase>> calculateScoreLogicTests;
extern std::shared_ptr<TestSuite> trustScoreTestSuite;
#endif
