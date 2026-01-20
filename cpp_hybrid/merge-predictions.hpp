#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_MERGE-PREDICTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_MERGE-PREDICTIONS_H
#include "core.h"
#include "path.h"
#include "fs.h"
#include "../utils/log.h"

extern std::shared_ptr<AgentLogger> logger;
void mergePredictions(array<string> directories, string output = undefined);

array<string> findPredFiles(string directory);

#endif
