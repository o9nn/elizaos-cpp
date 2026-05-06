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
    string name = std::string("starter");

    string description = std::string("E2E tests for the starter project");

    array<object> tests = array<object>{ object{
        object::pair{std::string("name"), std::string("Character configuration test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto requiredFields = array<string>{ std::string("name"), std::string("bio"), std::string("plugins"), std::string("system"), std::string("messageExamples") };
            auto missingFields = requiredFields->filter([=](auto field) mutable
            {
                return !(in(field, character));
            }
            );
            if (missingFields->get_length() > 0) {
                throw any(std::make_shared<Error>(std::string("Missing required fields: ") + missingFields->join(std::string(", ")) + string_empty));
            }
            if (character->name != std::string("Mr. TEE")) {
                throw any(std::make_shared<Error>(std::string("Expected character name to be 'Mr. TEE', got '") + character->name + std::string("'")));
            }
            if (!Array->isArray(character->plugins)) {
                throw any(std::make_shared<Error>(std::string("Character plugins should be an array")));
            }
            if (!character->system) {
                throw any(std::make_shared<Error>(std::string("Character system prompt is required")));
            }
            if (!Array->isArray(character->bio)) {
                throw any(std::make_shared<Error>(std::string("Character bio should be an array")));
            }
            if (!Array->isArray(character->messageExamples)) {
                throw any(std::make_shared<Error>(std::string("Character message examples should be an array")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Plugin initialization test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                std::async([=]() { runtime["registerPlugin"](object{
                    object::pair{std::string("name"), std::string("starter")}, 
                    object::pair{std::string("description"), std::string("A starter plugin for Eliza")}, 
                    object::pair{std::string("init"), [=]() mutable
                    {
                    }
                    }, 
                    object::pair{std::string("config"), object{}}
                }); });
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Failed to register plugin: ") + error["message"] + string_empty));
            }
        }
        }
    } };
};

#endif
