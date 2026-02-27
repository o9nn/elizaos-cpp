#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SCHEMAS_CHARACTER_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_CORE_SRC_SCHEMAS_CHARACTER_H
#include "core.hpp"
#include "zod.hpp"
#include "../types/agent.h"

class CharacterValidationResult;

extern any uuidSchema;
extern any contentSchema;
extern any messageExampleSchema;
extern any directoryItemSchema;
extern any knowledgeItemSchema;
extern any templateTypeSchema;
extern any styleSchema;
extern any settingsSchema;
extern any secretsSchema;
extern any characterSchema;
class CharacterValidationResult : public object, public std::enable_shared_from_this<CharacterValidationResult> {
public:
    using std::enable_shared_from_this<CharacterValidationResult>::shared_from_this;
    boolean success;

    std::shared_ptr<Character> data;

    object error;
};

std::shared_ptr<CharacterValidationResult> validateCharacter(any data);

std::shared_ptr<CharacterValidationResult> parseAndValidateCharacter(string jsonString);

any isValidCharacter(any data);

#endif
