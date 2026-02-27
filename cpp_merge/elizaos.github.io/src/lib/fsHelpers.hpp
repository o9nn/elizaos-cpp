#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_FSHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZAOS_GITHUB_IO_SRC_LIB_FSHELPERS_H
#include "core.hpp"
#include "fs/promises.h"
#include "path.hpp"

void ensureDir(string dirPath);

void writeToFile(string filePath, string data);

any getRepoFilePath(string outputDir, string repoId, string dataType, string intervalType, string fileName);

#endif
