#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CLI_SRC_COMMANDS_CREATE_ACTIONS_CREATORS_H
#include "core.hpp"
// External dependency removed
// External dependency removed
using copyTemplateUtil = copyTemplate;
#include "path.hpp"
// Node.js module removed - use C++ filesystem
// Using alias removed (invalid transpilation)
// External dependency removed
#include "yoctocolors.hpp"
// Using alias removed (invalid transpilation)
#include "../utils.h"
#include "./setup.h"

std::shared_ptr<Promise<void>> createPlugin(string pluginName, string targetDir, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createAgent(string agentName, string targetDir, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createTEEProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = std::nullopt, boolean isNonInteractive = false);

std::shared_ptr<Promise<void>> createProject(string projectName, string targetDir, string database, string aiModel, string embeddingModel = std::nullopt, boolean isNonInteractive = false);

#endif
