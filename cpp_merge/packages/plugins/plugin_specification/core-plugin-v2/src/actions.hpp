#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_ACTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_PLUGIN_SPECIFICATION_CORE_PLUGIN_V2_SRC_ACTIONS_H
#include "core.hpp"
// External dependency removed
using coreComposeActionExamples = composeActionExamples;
using coreFormatActionNames = formatActionNames;
using coreFormatActions = formatActions;

extern std::function<any(array<any>, double)> composeActionExamples;
string formatActionNames(array<std::shared_ptr<Action>> actions);

string formatActions(array<std::shared_ptr<Action>> actions);

#endif
