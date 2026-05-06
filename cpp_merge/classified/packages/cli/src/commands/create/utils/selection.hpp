#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_UTILS_SELECTION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_CLI_SRC_COMMANDS_CREATE_UTILS_SELECTION_H
#include "core.hpp"
// External dependency removed
#include "../types.h"

std::shared_ptr<Promise<array<string>>> getLocalAvailableDatabases();

array<std::shared_ptr<AIModelOption>> getAvailableAIModels();

boolean hasEmbeddingSupport(string aiModel);

array<std::shared_ptr<DatabaseOption>> getAvailableDatabases();

std::shared_ptr<Promise<string>> selectDatabase();

std::shared_ptr<Promise<string>> selectAIModel();

array<std::shared_ptr<AIModelOption>> getAvailableEmbeddingModels();

std::shared_ptr<Promise<string>> selectEmbeddingModel();

#endif
