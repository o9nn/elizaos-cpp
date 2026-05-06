#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_ACTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_PLUGIN-SPECIFICATION_CORE-PLUGIN-V2_SRC_ACTIONS_H
#include "core.h"
#include "@elizaos/core.h"
using coreComposeActionExamples = composeActionExamples;
using coreFormatActionNames = formatActionNames;
using coreFormatActions = formatActions;

extern std::function<std::any(array<any>, double)> composeActionExamples;
std::string formatActionNames(array<std::shared_ptr<Action>> actions);

std::string formatActions(array<std::shared_ptr<Action>> actions);

#endif
