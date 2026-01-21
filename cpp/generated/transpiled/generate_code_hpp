#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_GENERATE-CODE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-AUTOCODER_SRC_ACTIONS_GENERATE-CODE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/CodeGenerationService.h"
#include "../types/index.h"
#include "@elizaos/plugin-forms.h"

extern std::shared_ptr<Action> generateCodeAction;
std::shared_ptr<Promise<string>> extractProjectType(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message, std::shared_ptr<State> _state = undefined);

array<string> extractRequirements(string text);

array<string> extractAPIs(string text);

any extractProjectName(string text);

string getNextStepPrompt(any form, string completedStepId);

any extractProjectData(any formData);

#endif
