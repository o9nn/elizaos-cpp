#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_QUICK-STATS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_QUICK-STATS_H
#include "core.h"
#include "path.h"
#include "fs.h"
#include "../utils/log.h"

extern std::shared_ptr<AgentLogger> logger;
std::shared_ptr<Promise<string>> quickStats(string directory = std::string("."));

array<string> findTrajFiles(string directory);

#endif
