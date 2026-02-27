#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_COMPARE_RUNS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_COMPARE_RUNS_H
#include "core.hpp"
#include "path.hpp"
#include "fs.hpp"

std::shared_ptr<Set<string>> getResolved(string filePath);

std::shared_ptr<Set<string>> getSubmitted(string filePath);

void statsSingle(string filePath);

void compareMany(array<string> paths);

void comparePair(string newPath, string oldPath, boolean showSame = false);

std::shared_ptr<Promise<void>> compareRuns(array<string> paths, boolean showSame = false);

#endif
