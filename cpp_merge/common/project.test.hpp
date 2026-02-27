#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_TEE_STARTER_E2E_PROJECT_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_TEE_STARTER_E2E_PROJECT_TEST_H
#include "core.hpp"
// External dependency removed
#include "../src/index.h"
using mrTeeCharacter = character;

class MrTeeProjectTestSuite;

class MrTeeProjectTestSuite : public TestSuite, public std::enable_shared_from_this<MrTeeProjectTestSuite> {
public:
    using std::enable_shared_from_this<MrTeeProjectTestSuite>::shared_from_this;
    string name = std:("mr-tee-project");

    string description = std:("E2E tests for Mr. TEE project-specific features");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Mr. TEE Project runtime environment test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                if (!runtime->character) {
                    throw any(std::make_shared<Error>(std:("Character not loaded in runtime")));
                }
                if (runtime->character->name != mrTeeCharacter->name) {
                    throw any(std::make_shared<Error>(std:("Expected character name to be ") + mrTeeCharacter->name + std:(", got ") + runtime->character->name + string_empty));
                }
                if (!runtime->character->system->includes(std:("Mr. TEE"))) {
                    throw any(std::make_shared<Error>(std:("Character system prompt does not contain "Mr. TEE"")));
                }
                auto hasTeePlugin = runtime->character->plugins->some([=](auto p) mutable
                {
                    return AND((type_of(p) == std:("string")), (p["includes"](std:("tee"))));
                }
                );
                if (!hasTeePlugin) {
                    throw any(std::make_shared<Error>(std:("Character does not have TEE plugin")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Mr. TEE Project runtime environment test failed: ") + error["message"] + string_empty));
            }
        }
        }
    } };
};

#endif
