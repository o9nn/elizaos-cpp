#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-TEE-STARTER_E2E_PROJECT_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-TEE-STARTER_E2E_PROJECT_TEST_H
#include "core.h"
#include "@elizaos/core.h"
#include "../src/index.h"
using mrTeeCharacter = character;

class MrTeeProjectTestSuite;

class MrTeeProjectTestSuite : public TestSuite, public std::enable_shared_from_this<MrTeeProjectTestSuite> {
public:
    using std::enable_shared_from_this<MrTeeProjectTestSuite>::shared_from_this;
    string name = std::string("mr-tee-project");

    string description = std::string("E2E tests for Mr. TEE project-specific features");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Mr. TEE Project runtime environment test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                if (!runtime->character) {
                    throw any(std::make_shared<Error>(std::string("Character not loaded in runtime")));
                }
                if (runtime->character->name != mrTeeCharacter->name) {
                    throw any(std::make_shared<Error>(std::string("Expected character name to be ") + mrTeeCharacter->name + std::string(", got ") + runtime->character->name + string_empty));
                }
                if (!runtime->character->system->includes(std::string("Mr. TEE"))) {
                    throw any(std::make_shared<Error>(std::string("Character system prompt does not contain "Mr. TEE"")));
                }
                auto hasTeePlugin = runtime->character->plugins->some([=](auto p) mutable
                {
                    return AND((type_of(p) == std::string("string")), (p["includes"](std::string("tee"))));
                }
                );
                if (!hasTeePlugin) {
                    throw any(std::make_shared<Error>(std::string("Character does not have TEE plugin")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Mr. TEE Project runtime environment test failed: ") + error["message"] + string_empty));
            }
        }
        }
    } };
};

#endif
