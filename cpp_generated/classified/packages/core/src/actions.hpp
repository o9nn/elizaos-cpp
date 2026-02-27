#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_ACTIONS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_CLASSIFIED_PACKAGES_CORE_SRC_ACTIONS_H
#include "core.h"
#include "unique-names-generator.h"
#include "./types.h"

extern std::function<string(array<std::shared_ptr<Action>>, double)> composeActionExamples;
extern std::function<string(array<array<std::shared_ptr<ActionExample>>>)> formatSelectedExamples;
string formatActionNames(array<std::shared_ptr<Action>> actions);

string formatActions(array<std::shared_ptr<Action>> actions);

#endif
