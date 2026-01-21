#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_TESTS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_ELIZA-3D-HYPERFY-STARTER_SRC_TESTS_H
#include "core.h"
#include "@elizaos/core.h"
#include "uuid.h"
using uuidv4 = v4;
#include "./index.h"

class StarterTestSuite;

class StarterTestSuite : public TestSuite, public std::enable_shared_from_this<StarterTestSuite> {
public:
    using std::enable_shared_from_this<StarterTestSuite>::shared_from_this;
    string name = std::string("starter");

    string description = std::string("Tests for the starter project");

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
            if (character->name != std::string("Eliza")) {
                throw any(std::make_shared<Error>(std::string("Expected character name to be 'Eliza', got '") + character->name + std::string("'")));
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
                std::async([=]() { runtime->registerPlugin(object{
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
    }, object{
        object::pair{std::string("name"), std::string("Hello world action test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto message = object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Can you say hello?")}, 
                    object::pair{std::string("source"), std::string("test")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("HELLO_WORLD") }}
                }}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            shared responseReceived = false;
            try
            {
                std::async([=]() { runtime->processActions(message, array<any>(), state, [=](auto content) mutable
                {
                    if (AND((content->text == std::string("hello world!")), (content->actions->includes(std::string("HELLO_WORLD"))))) {
                        responseReceived = true;
                    }
                    return array<any>();
                }
                ); });
                if (!responseReceived) {
                    auto helloWorldAction = runtime->actions->find([=](auto a) mutable
                    {
                        return a["name"] == std::string("HELLO_WORLD");
                    }
                    );
                    if (helloWorldAction) {
                        std::async([=]() { helloWorldAction->handler(runtime, message, state, object{}, [=](auto content) mutable
                        {
                            if (AND((content->text == std::string("hello world!")), (content->actions->includes(std::string("HELLO_WORLD"))))) {
                                responseReceived = true;
                            }
                            return array<any>();
                        }
                        , array<any>()); });
                    } else {
                        throw any(std::make_shared<Error>(std::string("HELLO_WORLD action not found in runtime.actions")));
                    }
                }
                if (!responseReceived) {
                    throw any(std::make_shared<Error>(std::string("Hello world action did not produce expected response")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Hello world action test failed: ") + error["message"] + string_empty));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Hello world provider test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto message = object{
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(uuidv4())}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("What can you provide?")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}
            };
            auto state = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            try
            {
                if (OR((!runtime->providers), (runtime->providers->length == 0))) {
                    throw any(std::make_shared<Error>(std::string("No providers found in runtime")));
                }
                auto helloWorldProvider = runtime->providers->find([=](auto p) mutable
                {
                    return p["name"] == std::string("HELLO_WORLD_PROVIDER");
                }
                );
                if (!helloWorldProvider) {
                    throw any(std::make_shared<Error>(std::string("HELLO_WORLD_PROVIDER not found in runtime providers")));
                }
                auto result = std::async([=]() { helloWorldProvider->get(runtime, message, state); });
                if (result->text != std::string("I am a provider")) {
                    throw any(std::make_shared<Error>(std::string("Expected provider to return "I am a provider", got "") + result->text + std::string(""")));
                }
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Hello world provider test failed: ") + error["message"] + string_empty));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("Starter service test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            try
            {
                auto service = runtime->getService(std::string("starter"));
                if (!service) {
                    throw any(std::make_shared<Error>(std::string("Starter service not found")));
                }
                if (service->capabilityDescription != std::string("This is a starter service which is attached to the agent through the starter plugin.")) {
                    throw any(std::make_shared<Error>(std::string("Incorrect service capability description")));
                }
                std::async([=]() { service->stop(); });
            }
            catch (const any& error)
            {
                throw any(std::make_shared<Error>(std::string("Starter service test failed: ") + error["message"] + string_empty));
            }
        }
        }
    } };
};

#endif
