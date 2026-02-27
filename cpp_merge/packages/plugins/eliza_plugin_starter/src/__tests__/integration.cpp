#include "integration.test.h"

void Main(void)
{
    beforeAll([=]() mutable
    {
        setupLoggerSpies();
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std:("Integration: HelloWorld Action with StarterService"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            shared mockService = object{
                object::pair{std:("capabilityDescription"), std:("This is a starter service which is attached to the agent through the starter plugin.")}, 
                object::pair{std:("stop"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }
            };
            auto getServiceImpl = [=](auto serviceType) mutable
            {
                if (serviceType == std:("starter")) {
                    return mockService;
                }
                return nullptr;
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("getService"), getServiceImpl}
            });
        }
        );
        it(std:("should handle HelloWorld action with StarterService available"), [=]() mutable
        {
            auto helloWorldAction = starterPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std:("HELLO_WORLD");
            }
            );
            expect(helloWorldAction)->toBeDefined();
            auto mockMessage = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("Hello world")}, 
                    object::pair{std:("source"), std:("test")}
                }}, 
                object::pair{std:("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
            shared callbackCalls = array<any>();
            auto callbackFn = [=](Args... args_) mutable
            {
                any args = any{args_...};
                callbackCalls->push(args);
            };
            std::async([=]() { helloWorldAction->handler(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime)), mockMessage, mockState, object{}, as<std::shared_ptr<HandlerCallback>>(callbackFn), array<any>()); });
            expect(callbackCalls->get_length())->toBeGreaterThan(0);
            if (callbackCalls->get_length() > 0) {
                expect(const_(const_(callbackCalls)[0])[0])->toMatchObject(object{
                    object::pair{std:("text"), std:("hello world!")}, 
                    object::pair{std:("actions"), array<string>{ std:("HELLO_WORLD") }}
                });
            }
            auto service = mockRuntime->getService(std:("starter"));
            expect(service)->toBeDefined();
            expect(service["capabilityDescription"])->toContain(std:("starter service"));
        }
        );
    }
    );
    describe(std:("Integration: Plugin initialization and service registration"), [=]() mutable
    {
        it(std:("should initialize the plugin and register the service"), [=]() mutable
        {
            auto mockRuntime = createMockRuntime();
            shared registerServiceCalls = array<any>();
            mockRuntime->registerService = [=](auto type, auto service) mutable
            {
                registerServiceCalls->push(object{
                    object::pair{std:("type"), std:("type")}, 
                    object::pair{std:("service"), std:("service")}
                });
            };
            if (starterPlugin->init) {
                std::async([=]() { starterPlugin->init(object{
                    object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), std:("test-value")}
                }, as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                if (starterPlugin->services) {
                    auto StarterServiceClass = const_(starterPlugin->services)[0];
                    auto serviceInstance = std::async([=]() { StarterServiceClass->start(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                    mockRuntime->registerService(StarterServiceClass);
                }
                expect(registerServiceCalls->get_length())->toBeGreaterThan(0);
            }
        }
        );
    }
    );
}

MAIN
