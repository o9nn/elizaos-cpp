#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_QUICK_STATS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_QUICK_STATS_H
#include "core.hpp"
#include <string>
#include "path.hpp"
#include "fs.hpp"
#include "../utils/log.h"

extern std::shared_ptr<AgentLogger> logger;
std::shared_ptr<Promise<string>> quickStats(string directory = std::string("."));

array<string> findTrajFiles(string directory);

#endif
