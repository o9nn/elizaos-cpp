#include "starter-plugin.h"

std::shared_ptr<TestSuite> StarterPluginTestSuite = object{
    object::pair{std:("name"), std:("plugin_starter_test_suite")}, 
    object::pair{std:("description"), std:("E2E tests for the starter plugin")}, 
    object::pair{std:("tests"), array<object>{ object{
        object::pair{std:("name"), std:("example_test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            if (runtime["character"]["name"] != std:("Eliza")) {
                throw any(std::make_shared<Error>(std:("Expected character name to be "Eliza" but got "") + runtime["character"]["name"] + std:(""")));
            }
            if (!runtime["getService"]) {
                throw any(std::make_shared<Error>(std:("Runtime does not have getService method")));
            }
            auto service = OR((OR((runtime["getService"](std:("starter"))), (runtime["getService"](std:("_StarterService"))))), (runtime["getService"](std:("StarterService"))));
            if (AND((!service), (runtime["getAllServices"]))) {
                auto allServices = runtime["getAllServices"]();
                for (auto& [key, svc] : Object->entries(OR((allServices), (object{}))))
                {
                    if (key->toLowerCase()->includes(std:("starter"))) {
                        service = svc;
                        break;
                    }
                }
            }
            if (!service) {
                auto serviceList = (runtime["getAllServices"]) ? Object->keys(OR((runtime["getAllServices"]()), (object{}))) : array<any>();
                throw any(std::make_shared<Error>(std:("Starter service not found. Available services: ") + (OR((serviceList->join(std:(", "))), (std:("none")))) + string_empty));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("should_have_hello_world_action")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            auto actionExists = runtime["actions"]["some"]([=](auto a) mutable
            {
                return a["name"] == std:("HELLO_WORLD");
            }
            );
            if (!actionExists) {
                throw any(std::make_shared<Error>(std:("Hello world action not found in runtime actions")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("hello_world_action_test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            auto testMessage = object{
                object::pair{std:("entityId"), as<UUID>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("roomId"), as<UUID>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Can you say hello?")}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("actions"), array<string>{ std:("HELLO_WORLD") }}
                }}
            };
            auto testState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared responseText = string_empty;
            shared responseReceived = false;
            auto helloWorldAction = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std:("HELLO_WORLD");
            }
            );
            if (!helloWorldAction) {
                throw any(std::make_shared<Error>(std:("Hello world action not found in runtime actions")));
            }
            auto callback = [=](auto response) mutable
            {
                responseReceived = true;
                responseText = OR((response->text), (string_empty));
                if (!response->actions->includes(std:("HELLO_WORLD"))) {
                    throw any(std::make_shared<Error>(std:("Response did not include HELLO_WORLD action")));
                }
                return Promise->resolve(array<any>());
            };
            std::async([=]() { helloWorldAction["handler"](runtime, testMessage, testState, object{}, callback); });
            if (!responseReceived) {
                throw any(std::make_shared<Error>(std:("Hello world action did not produce a response")));
            }
            if (!responseText->toLowerCase()->includes(std:("hello world"))) {
                throw any(std::make_shared<Error>(std:("Expected response to contain "hello world" but got: "") + responseText + std:(""")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("hello_world_provider_test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            auto testMessage = object{
                object::pair{std:("entityId"), as<UUID>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("roomId"), as<UUID>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("What can you provide?")}, 
                    object::pair{std:("source"), std:("test")}
                }}
            };
            auto testState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            auto helloWorldProvider = runtime["providers"]["find"]([=](auto p) mutable
            {
                return p["name"] == std:("HELLO_WORLD_PROVIDER");
            }
            );
            if (!helloWorldProvider) {
                throw any(std::make_shared<Error>(std:("Hello world provider not found in runtime providers")));
            }
            auto result = std::async([=]() { helloWorldProvider["get"](runtime, testMessage, testState); });
            if (result["text"] != std:("I am a provider")) {
                throw any(std::make_shared<Error>(std:("Expected provider to return "I am a provider", got "") + result["text"] + std:(""")));
            }
        }
        }
    }, object{
        object::pair{std:("name"), std:("starter_service_test")}, 
        object::pair{std:("fn"), [=](auto runtime) mutable
        {
            if (!runtime["getService"]) {
                throw any(std::make_shared<Error>(std:("Runtime does not have getService method")));
            }
            auto service = OR((OR((runtime["getService"](std:("starter"))), (runtime["getService"](std:("_StarterService"))))), (runtime["getService"](std:("StarterService"))));
            if (AND((!service), (runtime["getAllServices"]))) {
                auto allServices = runtime["getAllServices"]();
                for (auto& [key, svc] : Object->entries(OR((allServices), (object{}))))
                {
                    if (key->toLowerCase()->includes(std:("starter"))) {
                        service = svc;
                        break;
                    }
                }
            }
            if (!service) {
                auto serviceList = (runtime["getAllServices"]) ? Object->keys(OR((runtime["getAllServices"]()), (object{}))) : array<any>();
                throw any(std::make_shared<Error>(std:("Starter service not found. Available services: ") + (OR((serviceList->join(std:(", "))), (std:("none")))) + string_empty));
            }
            if (service["capabilityDescription"] != std:("This is a starter service which is attached to the agent through the starter plugin.")) {
                throw any(std::make_shared<Error>(std:("Incorrect service capability description")));
            }
            std::async([=]() { service["stop"](); });
        }
        }
    } }}
};

void Main(void)
{
    string_empty + string + std:("-") + string + std:("-") + string + std:("-") + string + std:("-") + string + string_empty;
}

MAIN
