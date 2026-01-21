#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_TESTS_INTEGRATION_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-EXPERIENCE_TESTS_INTEGRATION_TEST_H
#include "core.h"
#include "vitest.h"
#include "../index.js.h"
#include "../service.js.h"
#include "../types.js.h"
#include "../evaluators/experienceEvaluator.js.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;

extern std::function<std::any()> tuuid;
extern std::shared_ptr<Provider> mockRAGProvider;
extern std::shared_ptr<Provider> mockRecentProvider;
extern std::any mockRuntime;
extern std::function<std::any(std::string, std::any)> createMockMessage;
extern std::function<std::any(std::any)> createMockState;
std::string detectDomain(std::string text);

#endif
