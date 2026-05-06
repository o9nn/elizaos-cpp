#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_UTILS_CHARACTER_UPDATER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_UTILS_CHARACTER_UPDATER_H
#include "core.hpp"
#include "jsonpath-plus.h"
// External dependency removed
#include "../types.h"

std::shared_ptr<Character> applyOperationsToCharacter(std::shared_ptr<Character> character, array<std::shared_ptr<ModificationOperation>> operations);

void addValue(any obj, string path, any value);

void modifyValue(any obj, string path, any value);

void deleteValue(any obj, string path);

boolean validateCharacterStructure(any character);

#endif
