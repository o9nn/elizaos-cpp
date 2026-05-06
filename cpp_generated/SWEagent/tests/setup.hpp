#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_SETUP_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TESTS_SETUP_H
#include "core.h"
#include "dotenv.h"
#include "path.h"

extern std::function<array<any>()> createMockHistory;
extern std::function<std::shared_ptr<Promise<any>>(double)> delay;
#endif
