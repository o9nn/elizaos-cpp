#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_LOCAL_CLI_DELEGATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_LOCAL_CLI_DELEGATION_H
#include "core.hpp"
#include "node:child_process.h"
#include "node:fs.h"
#include "node:path.h"
// Using alias removed (invalid transpilation)
#include "node:url.h"
// External dependency removed

boolean isRunningFromLocalCli();

any getLocalCliPath();

Record<string, string> setupLocalEnvironment();

std::shared_ptr<Promise<void>> delegateToLocalCli(string localCliPath);

boolean isTestOrCiEnvironment();

std::shared_ptr<Promise<boolean>> tryDelegateToLocalCli();

boolean hasLocalCli();

object getCliContext();

#endif
