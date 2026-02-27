#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_MERGE_PREDICTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_MERGE_PREDICTIONS_H
#include "core.hpp"
#include "path.hpp"
#include "fs.hpp"
#include "../utils/log.h"

extern std::shared_ptr<AgentLogger> logger;
void mergePredictions(array<string> directories, string output = undefined);

array<string> findPredFiles(string directory);

#endif
