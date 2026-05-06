#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#include "core.h"
#include "@/src/characters/eliza.h"
#include "@/src/utils.h"
using copyTemplateUtil = copyTemplate;
#include "path.h"
#include "node:fs/promises.h"
using fs = _default;
#include "@clack/prompts.h"
#include "yoctocolors.h"
using colors = _default;
#include "../utils.h"
#include "./setup.h"

std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createAgent(string agentName, string targetDir, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = undefined, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = undefined, boolean isNonInteractive = false);

#endif
