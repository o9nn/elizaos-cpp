#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PERSONALITY_SRC_ACTIONS_MODIFY-CHARACTER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-PERSONALITY_SRC_ACTIONS_MODIFY-CHARACTER_H
#include "core.h"
#include "@elizaos/core.h"
#include "../services/character-file-manager.h"
#include "../utils/json-parser.h"

extern std::shared_ptr<Action> modifyCharacterAction;
std::shared_ptr<Promise<object>> detectModificationIntent(std::shared_ptr<IAgentRuntime> runtime, string messageText);

std::shared_ptr<Promise<any>> parseUserModificationRequest(std::shared_ptr<IAgentRuntime> runtime, string messageText);

std::shared_ptr<Promise<object>> evaluateModificationSafety(std::shared_ptr<IAgentRuntime> runtime, any modification, string requestText);

std::shared_ptr<Promise<boolean>> checkAdminPermissions(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<Memory> message);

string summarizeModification(any modification);

#endif
