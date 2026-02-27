#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_TESTS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_ELIZA_3D_HYPERFY_STARTER_SRC_TESTS_H
#include "core.hpp"
// External dependency removed
#include "uuid.hpp"
using uuidv4 = v4;
#include "./index.h"

class StarterTestSuite;

class StarterTestSuite : public TestSuite, public std::enable_shared_from_this<StarterTestSuite> {
public:
    using std::enable_shared_from_this<StarterTestSuite>::shared_from_this;
    string name = std:("starter");

    string description = std:("Tests for the starter project");

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
            if (character->name != std:("Eliza")) {
                throw any(std::make_shared<Error>(std:("Expected character name to be 'Eliza', got '") + character->name + std:("'")));
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
                std::async([=]() { runtime->registerPlugin(object{
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
    }, object{
        object::pair{std:("name"), std:("Hello world action test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            auto message = object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Can you say hello?")}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("actions"), array<string>{ std:("HELLO_WORLD") }}
                }}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared responseReceived = false;
            try
            {
                std::async([=]() { runtime->processActions(message, array<any>(), state, [=](auto content) mutable
                {
                    if (AND((content->text == std:("hello world!")), (content->actions->includes(std:("HELLO_WORLD"))))) {
                        responseReceived = true;
                    }
                    return array<any>();
                }
                ); });
                if (!responseReceived) {
                    auto helloWorldAction = runtime->actions->find([=](auto a) mutable
                    {
                        return a["name"] == std:("HELLO_WORLD");
                    }
                    );
                    if (helloWorldAction) {
                        std::async([=]() { helloWorldAction->handler(runtime, message, state, object{}, [=](auto content) mutable
                        {
                            if (AND((content->text == std:("hello world!")), (content->actions->includes(std:("HELLO_WORLD"))))) {
                                responseReceived = true;
                            }
                            return array<any>();
                        }
                        , array<any>()); });
                    } else {
                        throw any(std::make_shared<Error>(std:("HELLO_WORLD action not found in runtime.actions")));
                    }
                }
                if (!responseReceived) {
                    throw any(std::make_shared<Error>(std:("Hello world action did not produce expected response")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Hello world action test failed: ") + error["message"] + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Hello world provider test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            auto message = object{
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("What can you provide?")}, 
                    object::pair{std:("source"), std:("test")}
                }}
            };
            auto state = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            try
            {
                if (OR((!runtime->providers), (runtime->providers->length == 0))) {
                    throw any(std::make_shared<Error>(std:("No providers found in runtime")));
                }
                auto helloWorldProvider = runtime->providers->find([=](auto p) mutable
                {
                    return p["name"] == std:("HELLO_WORLD_PROVIDER");
                }
                );
                if (!helloWorldProvider) {
                    throw any(std::make_shared<Error>(std:("HELLO_WORLD_PROVIDER not found in runtime providers")));
                }
                auto result = std::async([=]() { helloWorldProvider->get(runtime, message, state); });
                if (result->text != std:("I am a provider")) {
                    throw any(std::make_shared<Error>(std:("Expected provider to return "I am a provider", got "") + result->text + std:(""")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Hello world provider test failed: ") + error["message"] + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("Starter service test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            try
            {
                auto service = runtime->getService(std:("starter"));
                if (!service) {
                    throw any(std::make_shared<Error>(std:("Starter service not found")));
                }
                if (service->capabilityDescription != std:("This is a starter service which is attached to the agent through the starter plugin.")) {
                    throw any(std::make_shared<Error>(std:("Incorrect service capability description")));
                }
                std::async([=]() { service->stop(); });
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std:("Starter service test failed: ") + error["message"] + string_empty));
            }
        }
        }
    } };
};

#endif
