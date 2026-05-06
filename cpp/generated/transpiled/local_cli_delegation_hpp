#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_LOCAL-CLI-DELEGATION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_LOCAL-CLI-DELEGATION_H
#include "core.h"
#include "node:child_process.h"
#include "node:fs.h"
#include "node:path.h"
using path = _default;
#include "node:url.h"
#include "@elizaos/core.h"

boolean isRunningFromLocalCli();

any getLocalCliPath();

Record<string, string> setupLocalEnvironment();

std::shared_ptr<Promise<void>> delegateToLocalCli(string localCliPath);

boolean isTestOrCiEnvironment();

std::shared_ptr<Promise<boolean>> tryDelegateToLocalCli();

boolean hasLocalCli();

object getCliContext();

#endif
