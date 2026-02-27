#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_UTILS_VALIDATION_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_AUTONOMOUS_STARTER_SRC_PLUGIN_SELF_MODIFICATION_SRC_UTILS_VALIDATION_H
#include "core.hpp"
#include "../types.h"

extern array<string> IMMUTABLE_FIELDS;
extern double MAX_BIO_LENGTH;
extern double MAX_ARRAY_ITEMS;
extern double MAX_SYSTEM_PROMPT_LENGTH;
std::shared_ptr<ValidationResult> validateCharacterDiff(std::shared_ptr<CharacterDiff> diff);

boolean validateDataType(any value, string expectedType);

boolean validateModificationRate(array<object> recentModifications, double maxPerHour = 5, double maxPerDay = 20);

#endif
