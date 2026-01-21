#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_FSHELPERS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZAOS_GITHUB_IO_SRC_LIB_FSHELPERS_H
#include "core.h"
#include "fs/promises.h"
#include "path.h"

void ensureDir(std::string dirPath);

void writeToFile(std::string filePath, std::string data);

std::any getRepoFilePath(std::string outputDir, std::string repoId, std::string dataType, std::string intervalType, std::string fileName);

#endif
