#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_UTILS_SELECTION_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_UTILS_SELECTION_H
#include "core.h"
#include "@clack/prompts.h"
#include "../types.h"

std::shared_ptr<Promise<array<string>>> getLocalAvailableDatabases();

array<std::shared_ptr<AIModelOption>> getAvailableAIModels();

array<std::shared_ptr<DatabaseOption>> getAvailableDatabases();

std::shared_ptr<Promise<string>> selectDatabase();

std::shared_ptr<Promise<string>> selectAIModel();

array<std::shared_ptr<AIModelOption>> getAvailableEmbeddingModels();

std::shared_ptr<Promise<string>> selectEmbeddingModel();

#endif
