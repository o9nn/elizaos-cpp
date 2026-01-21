#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_COMPARE-RUNS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_COMPARE-RUNS_H
#include "core.h"
#include "path.h"
#include "fs.h"

std::shared_ptr<Set<string>> getResolved(std::string filePath);

std::shared_ptr<Set<string>> getSubmitted(std::string filePath);

void statsSingle(std::string filePath);

void compareMany(array<string> paths);

void comparePair(std::string newPath, std::string oldPath, boolean showSame = false);

std::shared_ptr<Promise<void>> compareRuns(array<string> paths, boolean showSame = false);

#endif
