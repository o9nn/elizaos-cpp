#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-starter/src/__tests__/integration.test.h"

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
    describe(std::string("Integration: HelloWorld Action with StarterService"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            shared mockService = object{
                object::pair{std::string("capabilityDescription"), std::string("This is a starter service which is attached to the agent through the starter plugin.")}, 
                object::pair{std::string("stop"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }
            };
            auto getServiceImpl = [=](auto serviceType) mutable
            {
                if (serviceType == std::string("starter")) {
                    return mockService;
                }
                return nullptr;
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("getService"), getServiceImpl}
            });
        }
        );
        it(std::string("should handle HelloWorld action with StarterService available"), [=]() mutable
        {
            auto helloWorldAction = starterPlugin->actions->find([=](auto action) mutable
            {
                return action["name"] == std::string("HELLO_WORLD");
            }
            );
            expect(helloWorldAction)->toBeDefined();
            auto mockMessage = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("12345678-1234-1234-1234-123456789012"))}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("Hello world")}, 
                    object::pair{std::string("source"), std::string("test")}
                }}, 
                object::pair{std::string("createdAt"), Date->now()}
            };
            auto mockState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
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
                    object::pair{std::string("text"), std::string("hello world!")}, 
                    object::pair{std::string("actions"), array<string>{ std::string("HELLO_WORLD") }}
                });
            }
            auto service = mockRuntime->getService(std::string("starter"));
            expect(service)->toBeDefined();
            expect(service["capabilityDescription"])->toContain(std::string("starter service"));
        }
        );
    }
    );
    describe(std::string("Integration: Plugin initialization and service registration"), [=]() mutable
    {
        it(std::string("should initialize the plugin and register the service"), [=]() mutable
        {
            auto mockRuntime = createMockRuntime();
            shared registerServiceCalls = array<any>();
            mockRuntime->registerService = [=](auto type, auto service) mutable
            {
                registerServiceCalls->push(object{
                    object::pair{std::string("type"), std::string("type")}, 
                    object::pair{std::string("service"), std::string("service")}
                });
            };
            if (starterPlugin->init) {
                std::async([=]() { starterPlugin->init(object{
                    object::pair{std::string("EXAMPLE_PLUGIN_VARIABLE"), std::string("test-value")}
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
