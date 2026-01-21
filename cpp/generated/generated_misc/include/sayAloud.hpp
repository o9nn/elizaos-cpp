#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SAM_SRC_ACTIONS_SAYALOUD_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_PLUGIN-SAM_SRC_ACTIONS_SAYALOUD_H
#include "core.h"
#include "@elizaos/core.h"
#include "@elizaos/core.h"
#include "../services/SamTTSService.h"

extern std::shared_ptr<Action> sayAloudAction;
std::string extractTextToSpeak(std::string messageText);

std::shared_ptr<SamTTSOptions> extractVoiceOptions(std::string messageText);

#endif
