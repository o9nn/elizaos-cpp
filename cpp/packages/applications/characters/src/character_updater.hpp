#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_UTILS_CHARACTER-UPDATER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_UTILS_CHARACTER-UPDATER_H
#include "core.h"
#include "jsonpath-plus.h"
#include "@elizaos/core.h"
#include "../types.h"

std::shared_ptr<Character> applyOperationsToCharacter(std::shared_ptr<Character> character, array<std::shared_ptr<ModificationOperation>> operations);

void addValue(any obj, string path, any value);

void modifyValue(any obj, string path, any value);

void deleteValue(any obj, string path);

boolean validateCharacterStructure(any character);

#endif
