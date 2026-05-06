#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___E2E_PROJECT_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-STARTER_SRC___TESTS___E2E_PROJECT_TEST_H
#include "core.h"
#include "@elizaos/core.h"

class ProjectTestSuite;

class ProjectTestSuite : public TestSuite, public std::enable_shared_from_this<ProjectTestSuite> {
public:
    using std::enable_shared_from_this<ProjectTestSuite>::shared_from_this;
    string name = std::string("project");

    string description = std::string("E2E tests for project-specific features");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Project runtime environment test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                if (!runtime["character"]) {
                    throw any(std::make_shared<Error>(std::string("Character not loaded in runtime")));
                }
                auto character = runtime["character"];
                if (!character["name"]) {
                    throw any(std::make_shared<Error>(std::string("Character name is missing")));
                }
                if (character["name"] != std::string("Eliza")) {
                    throw any(std::make_shared<Error>(std::string("Expected character name 'Eliza', got '") + character["name"] + std::string("'")));
                }
                if (!character["system"]) {
                    throw any(std::make_shared<Error>(std::string("Character system prompt is missing")));
                }
                if (!Array->isArray(character["bio"])) {
                    throw any(std::make_shared<Error>(std::string("Character bio should be an array")));
                }
                if (!Array->isArray(character["messageExamples"])) {
                    throw any(std::make_shared<Error>(std::string("Character message examples should be an array")));
                }
                if (AND((character["plugins"]), (!Array->isArray(character["plugins"])))) {
                    throw any(std::make_shared<Error>(std::string("Character plugins should be an array")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Project runtime environment test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    } };
};

#endif
