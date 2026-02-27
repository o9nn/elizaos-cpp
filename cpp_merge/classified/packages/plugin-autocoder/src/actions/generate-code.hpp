#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC_ACTIONS_GENERATE_CODE_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_AUTOCODER_SRC_ACTIONS_GENERATE_CODE_H
#include "core.hpp"
// External dependency removed
#include "../services/CodeGenerationService.h"
#include "../types/index.h"
// External dependency removed

extern std::shared_ptr<Action> generateCodeAction;
std::shared_ptr<Promise<string>> extractProjectType(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> _state = std::nullopt);

array<string> extractRequirements(string text);

array<string> extractAPIs(string text);

any extractProjectName(string text);

string getNextStepPrompt(any form, string completedStepId);

any extractProjectData(any formData);

#endif
