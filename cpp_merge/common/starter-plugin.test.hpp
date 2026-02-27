#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-TEE-STARTER_E2E_STARTER-PLUGIN_TEST_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA_PACKAGES_PROJECT-TEE-STARTER_E2E_STARTER-PLUGIN_TEST_H
#include "core.h"
#include "../src/index.h"
#include "uuid.h"
using uuidv4 = v4;

typedef  UUID;

class TestSuite;
class Memory;
class State;
class Content;
class StarterTestSuite;

class TestSuite : public object, public std::enable_shared_from_this<TestSuite> {
public:
    using std::enable_shared_from_this<TestSuite>::shared_from_this;
    string name;

    string description;

    array<object> tests;
};

class Memory : public object, public std::enable_shared_from_this<Memory> {
public:
    using std::enable_shared_from_this<Memory>::shared_from_this;
    UUID entityId;

    UUID roomId;

    object content;
};

class State : public object, public std::enable_shared_from_this<State> {
public:
    using std::enable_shared_from_this<State>::shared_from_this;
    Record<string, any> values;

    Record<string, any> data;

    string text;
};

class Content : public object, public std::enable_shared_from_this<Content> {
public:
    using std::enable_shared_from_this<Content>::shared_from_this;
    string text;

    string source;

    array<string> actions;
};

class StarterTestSuite : public TestSuite, public std::enable_shared_from_this<StarterTestSuite> {
public:
    using std::enable_shared_from_this<StarterTestSuite>::shared_from_this;
    string name = std:("starter");

    string description = std:("E2E tests for the starter project");

    array<object> tests = array<object>{ object{
        object::pair{std:("name"), std:("Character configuration test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            auto requiredFields = array<string>{ std:("name"), std:("bio"), std:("plugins"), std:("system"), std:("messageExamples") };
            auto missingFields = requiredFields->filter([=](auto field) mutable
            {
                return !(in(field, character));
            }
            );
            if (missingFields->get_length() > 0) {
                throw any(std::make_shared<Error>(std:("Missing required fields: ") + missingFields->join(std:(", ")) + string_empty));
            }
            if (character->name != std:("Mr. TEE")) {
                throw any(std::make_shared<Error>(std:("Expected character name to be 'Mr. TEE', got '") + character->name + std:("'")));
            }
            if (!Array->isArray(character->plugins)) {
                throw any(std::make_shared<Error>(std:("Character plugins should be an array")));
            }
            if (!character->system) {
                throw any(std::make_shared<Error>(std:("Character system prompt is required")));
            }
            if (!Array->isArray(character->bio)) {
                throw any(std::make_shared<Error>(std:("Character bio should be an array")));
            }
            if (!Array->isArray(character->messageExamples)) {
                throw any(std::make_shared<Error>(std:("Character message examples should be an array")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Plugin initialization test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                std::async([=]() { runtime["registerPlugin"](object{
                    object::pair{std:("name"), std:("starter")}, 
                    object::pair{std:("description"), std:("A starter plugin for Eliza")}, 
                    object::pair{std:("init"), [=]() mutable
                    {
                    }
                    }, 
                    object::pair{std:("config"), object{}}
                }); });
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Failed to register plugin: ") + error["message"] + string_empty));
            }
        }
        }
    } };
};

#endif
