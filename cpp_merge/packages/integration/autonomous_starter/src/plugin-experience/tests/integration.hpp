#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_EXPERIENCE_TESTS_INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_EXPERIENCE_TESTS_INTEGRATION_TEST_H
#include "core.hpp"
#include "vitest.hpp"
#include "../index.js.h"
#include "../service.js.h"
#include "../types.js.h"
#include "../evaluators/experienceEvaluator.js.h"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;

extern std::function<any()> tuuid;
extern std::shared_ptr<Provider> mockRAGProvider;
extern std::shared_ptr<Provider> mockRecentProvider;
extern any mockRuntime;
extern std::function<any(string, any)> createMockMessage;
extern std::function<any(any)> createMockState;
string detectDomain(string text);

#endif
