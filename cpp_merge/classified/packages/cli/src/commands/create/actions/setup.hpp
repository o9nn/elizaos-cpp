#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_SETUP_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_SETUP_H
#include "core.hpp"
// Node.js module removed - use C++ filesystem
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// External dependency removed
// External dependency removed

std::shared_ptr<Promise<void>> createProjectDirectories(string targetDir);

std::shared_ptr<Promise<void>> setupAIModelConfig(string aiModel, string envFilePath, boolean isNonInteractive = false);

boolean hasValidApiKey(string content, string keyName);

std::shared_ptr<Promise<void>> setupEmbeddingModelConfig(string embeddingModel, string envFilePath, boolean isNonInteractive = false);

any resolveModelToPlugin(string modelName);

std::shared_ptr<Promise<void>> installModelPlugin(string modelName, string targetDir, string purpose = string_empty);

std::shared_ptr<Promise<void>> setupProjectEnvironment(string targetDir, string database, string aiModel, string embeddingModel = std::nullopt, boolean isNonInteractive = false);

#endif
