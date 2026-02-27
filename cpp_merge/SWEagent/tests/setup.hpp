#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TESTS_SETUP_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_TESTS_SETUP_H
#include "core.hpp"
#include "dotenv.hpp"
#include "path.hpp"

extern std::function<array<any>()> createMockHistory;
extern std::function<std::shared_ptr<Promise<any>>(double)> delay;
#endif
