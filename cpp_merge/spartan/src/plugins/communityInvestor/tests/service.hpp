#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_TESTS_SERVICE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SPARTAN_SRC_PLUGINS_COMMUNITYINVESTOR_TESTS_SERVICE_H
#include "core.hpp"
// External dependency removed
using CoreComponent = Component;
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "../service.h"
#include "../types.h"

extern any testUserIdGlobalForService;
extern std::function<any(any, any, any)> createFullMockComponentForSvcTest;
extern std::function<any(string, double, any, any, any, double, any)> createMockRecForSvcTest;
extern array<std::shared_ptr<TestCase>> calculateUserTrustScoreTestCases;
extern array<std::shared_ptr<TestCase>> resolveTickerTestCases;
extern array<std::shared_ptr<TestCase>> getTokenAPIDataTestCases;
extern array<std::shared_ptr<TestCase>> isLikelyScamOrRugTestCases;
extern array<std::shared_ptr<TestCase>> evaluatePerfTestCases;
extern array<std::shared_ptr<TestCase>> getLeaderboardDataTestCases;
extern std::shared_ptr<TestSuite> serviceTestSuite;
#endif
