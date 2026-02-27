#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RULES_CLI_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RULES_CLI_H
#include "core.hpp"
#include "commander.hpp"
#include "path.hpp"
#include "fs.hpp"
#include "./validators.h"
#include "./config.h"
#include "./general.h"

extern any program;
std::shared_ptr<Promise<array<string>>> findProjectFiles();

std::shared_ptr<Promise<array<string>>> resolveFiles(array<string> patterns);

std::shared_ptr<Promise<array<string>>> findFilesInDir(string dir);

#endif
