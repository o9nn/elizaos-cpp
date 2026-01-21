#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_GENERATE-CODE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_GENERATE-CODE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/CodeGenerationService.h"
#include "../types/index.h"
#include "@elizaos/plugin-forms.h"

extern std::shared_ptr<Action> generateCodeAction;
std::shared_ptr<Promise<string>> extractProjectType(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> _state = undefined);

array<string> extractRequirements(std::string text);

array<string> extractAPIs(std::string text);

std::any extractProjectName(std::string text);

std::string getNextStepPrompt(std::any form, std::string completedStepId);

std::any extractProjectData(std::any formData);

#endif
