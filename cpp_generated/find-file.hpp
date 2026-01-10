#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TOOLS_SRC_SEARCH_FIND-FILE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_TOOLS_SRC_SEARCH_FIND-FILE_H
#include "core.h"
#include "fs.h"
#include "path.h"
#include "child_process.h"
#include "commander.h"
#include "glob.h"

void findFile(string fileName, string dir = std::string("./"));

void setupCLI();

#endif
