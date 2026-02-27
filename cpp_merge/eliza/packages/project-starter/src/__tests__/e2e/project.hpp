#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_STARTER_SRC___TESTS___E2E_PROJECT_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_PACKAGES_PROJECT_STARTER_SRC___TESTS___E2E_PROJECT_TEST_H
#include "core.hpp"
// External dependency removed

class ProjectTestSuite;

class ProjectTestSuite : public TestSuite, public std::enable_shared_from_this<ProjectTestSuite> {
public:
    using std::enable_shared_from_this<ProjectTestSuite>::shared_from_this;
    string name = std:("project");

    string description = std:("E2E tests for project-specific features");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Project runtime environment test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                if (!runtime["character"]) {
                    throw any(std::make_shared<Error>(std:("Character not loaded in runtime")));
                }
                auto character = runtime["character"];
                if (!character["name"]) {
                    throw any(std::make_shared<Error>(std:("Character name is missing")));
                }
                if (character["name"] != std:("Eliza")) {
                    throw any(std::make_shared<Error>(std:("Expected character name 'Eliza', got '") + character["name"] + std:("'")));
                }
                if (!character["system"]) {
                    throw any(std::make_shared<Error>(std:("Character system prompt is missing")));
                }
                if (!Array->isArray(character["bio"])) {
                    throw any(std::make_shared<Error>(std:("Character bio should be an array")));
                }
                if (!Array->isArray(character["messageExamples"])) {
                    throw any(std::make_shared<Error>(std:("Character message examples should be an array")));
                }
                if (AND((character["plugins"]), (!Array->isArray(character["plugins"])))) {
                    throw any(std::make_shared<Error>(std:("Character plugins should be an array")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Project runtime environment test failed: ") + (as<std::shared_ptr<Error>>(error))->message + string_empty));
            }
        }
        }
    } };
};

#endif
