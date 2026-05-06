#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_FSHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_FSHELPERS_H
#include "core.h"
#include "fs/promises.h"
#include "path.h"

void ensureDir(string dirPath);

void writeToFile(string filePath, string data);

any getRepoFilePath(string outputDir, string repoId, string dataType, string intervalType, string fileName);

#endif
