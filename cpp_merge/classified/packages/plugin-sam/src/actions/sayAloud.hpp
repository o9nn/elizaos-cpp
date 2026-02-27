#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SAM_SRC_ACTIONS_SAYALOUD_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_CLASSIFIED_PACKAGES_PLUGIN_SAM_SRC_ACTIONS_SAYALOUD_H
#include "core.hpp"
// External dependency removed
// External dependency removed
#include "../services/SamTTSService.h"

extern std::shared_ptr<Action> sayAloudAction;
string extractTextToSpeak(string messageText);

std::shared_ptr<SamTTSOptions> extractVoiceOptions(string messageText);

#endif
