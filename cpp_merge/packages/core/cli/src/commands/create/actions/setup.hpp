#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_SETUP_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_SETUP_H
#include "core.hpp"
#include "node:fs.h"
#include "node:fs/promises.h"
// Using alias removed (invalid transpilation)
// External dependency removed

std::shared_ptr<Promise<void>> createProjectDirectories(string targetDir);

std::shared_ptr<Promise<void>> setupAIModelConfig(string aiModel, string envFilePath, boolean isNonInteractive = false);

boolean hasValidApiKey(string content, string keyName);

std::shared_ptr<Promise<void>> setupEmbeddingModelConfig(string embeddingModel, string envFilePath, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> installDependencies(string targetDir);

std::shared_ptr<Promise<void>> setupProjectEnvironment(string targetDir, string database, string aiModel, string embeddingModel = undefined, boolean isNonInteractive = false);

#endif
