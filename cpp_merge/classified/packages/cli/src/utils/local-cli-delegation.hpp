#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_LOCAL_CLI_DELEGATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_UTILS_LOCAL_CLI_DELEGATION_H
#include "core.hpp"
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// Node.js module removed - use C++ filesystem
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
