#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_UTILS_CHARACTER-UPDATER_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_AUTONOMOUS-STARTER_SRC_PLUGIN-SELF-MODIFICATION_SRC_UTILS_CHARACTER-UPDATER_H
#include "core.h"
#include "jsonpath-plus.h"
#include "@elizaos/core.h"
#include "../types.h"

std::shared_ptr<Character> applyOperationsToCharacter(std::shared_ptr<Character> character, array<std::shared_ptr<ModificationOperation>> operations);

void addValue(std::any obj, std::string path, std::any value);

void modifyValue(std::any obj, std::string path, std::any value);

void deleteValue(std::any obj, std::string path);

boolean validateCharacterStructure(std::any character);

#endif
