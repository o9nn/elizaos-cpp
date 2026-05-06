#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-starter/src/__tests__/e2e/starter-plugin.h"

std::shared_ptr<TestSuite> StarterPluginTestSuite = object{
    object::pair{std::string("name"), std::string("plugin_starter_test_suite")}, 
    object::pair{std::string("description"), std::string("E2E tests for the starter plugin")}, 
    object::pair{std::string("tests"), array<object>{ object{
        object::pair{std::string("name"), std::string("example_test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            if (runtime["character"]["name"] != std::string("Eliza")) {
                throw any(std::make_shared<Error>(std::string("Expected character name to be "Eliza" but got "") + runtime["character"]["name"] + std::string(""")));
            }
            auto service = runtime["getService"](std::string("starter"));
            if (!service) {
                throw any(std::make_shared<Error>(std::string("Starter service not found")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("should_have_hello_world_action")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto actionExists = runtime["actions"]["some"]([=](auto a) mutable
            {
                return a["name"] == std::string("HELLO_WORLD");
            }
            );
            if (!actionExists) {
                throw any(std::make_shared<Error>(std::string("Hello world action not found in runtime actions")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("hello_world_action_test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto testMessage = object{
                object::pair{std::string("entityId"), as<UUID>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("roomId"), as<UUID>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Can you say hello?")}, 
                    object::pair{std::string("source"), std::string("test")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("HELLO_WORLD") }}
                }}
            };
            auto testState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            shared responseText = string_empty;
            shared responseReceived = false;
            auto helloWorldAction = runtime["actions"]["find"]([=](auto a) mutable
            {
                return a["name"] == std::string("HELLO_WORLD");
            }
            );
            if (!helloWorldAction) {
                throw any(std::make_shared<Error>(std::string("Hello world action not found in runtime actions")));
            }
            auto callback = [=](auto response) mutable
            {
                responseReceived = true;
                responseText = OR((response->text), (string_empty));
                if (!response->actions->includes(std::string("HELLO_WORLD"))) {
                    throw any(std::make_shared<Error>(std::string("Response did not include HELLO_WORLD action")));
                }
                return Promise->resolve(array<any>());
            };
            std::async([=]() { helloWorldAction["handler"](runtime, testMessage, testState, object{}, callback); });
            if (!responseReceived) {
                throw any(std::make_shared<Error>(std::string("Hello world action did not produce a response")));
            }
            if (!responseText->toLowerCase()->includes(std::string("hello world"))) {
                throw any(std::make_shared<Error>(std::string("Expected response to contain "hello world" but got: "") + responseText + std::string(""")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("hello_world_provider_test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto testMessage = object{
                object::pair{std::string("entityId"), as<UUID>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("roomId"), as<UUID>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("What can you provide?")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}
            };
            auto testState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
            auto helloWorldProvider = runtime["providers"]["find"]([=](auto p) mutable
            {
                return p["name"] == std::string("HELLO_WORLD_PROVIDER");
            }
            );
            if (!helloWorldProvider) {
                throw any(std::make_shared<Error>(std::string("Hello world provider not found in runtime providers")));
            }
            auto result = std::async([=]() { helloWorldProvider["get"](runtime, testMessage, testState); });
            if (result["text"] != std::string("I am a provider")) {
                throw any(std::make_shared<Error>(std::string("Expected provider to return "I am a provider", got "") + result["text"] + std::string(""")));
            }
        }
        }
    }, object{
        object::pair{std::string("name"), std::string("starter_service_test")}, 
        object::pair{std::string("fn"), [=](auto runtime) mutable
        {
            auto service = runtime["getService"](std::string("starter"));
            if (!service) {
                throw any(std::make_shared<Error>(std::string("Starter service not found")));
            }
            if (service["capabilityDescription"] != std::string("This is a starter service which is attached to the agent through the starter plugin.")) {
                throw any(std::make_shared<Error>(std::string("Incorrect service capability description")));
            }
            std::async([=]() { service["stop"](); });
        }
        }
    } }}
};

void Main(void)
{
    string_empty + string + std::string("-") + string + std::string("-") + string + std::string("-") + string + std::string("-") + string + string_empty;
}

MAIN
