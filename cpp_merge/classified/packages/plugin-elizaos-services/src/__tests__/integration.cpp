#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/__tests__/integration.test.h"

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
    describe(std:("Integration: ElizaOS Models with ElizaOSService"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            shared mockService = object{
                object::pair{std:("capabilityDescription"), std:("ElizaOS hosted AI inference and storage services with multi-provider support")}, 
                object::pair{std:("stop"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }
            };
            auto getServiceImpl = [=](auto serviceType) mutable
            {
                if (serviceType == std:("elizaos-services")) {
                    return mockService;
                }
                return nullptr;
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std:("getService"), mock()->mockImplementation(getServiceImpl)}
            });
        }
        );
        it(std:("should handle TEXT_SMALL model with ElizaOSService available"), [=]() mutable
        {
            auto textSmallModel = elizaOSServicesPlugin->models->TEXT_SMALL;
            expect(textSmallModel)->toBeDefined();
            try
            {
                std::async([=]() { textSmallModel(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime)), object{
                    object::pair{std:("prompt"), std:("test prompt")}, 
                    object::pair{std:("maxTokens"), 10}, 
                    object::pair{std:("temperature"), 0.5}
                }); });
            }
            catch (const any& error)
            {
                expect((as<std::shared_ptr<Error>>(error))->message)->toContain(std:("No API provider available"));
            }
            auto service = mockRuntime->getService(std:("elizaos-services"));
            expect(service)->toBeDefined();
            expect(service["capabilityDescription"])->toContain(std:("ElizaOS"));
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
            mockRuntime->registerService = mock()->mockImplementation([=](auto type, auto service) mutable
            {
                registerServiceCalls->push(object{
                    object::pair{std:("type"), std:("type")}, 
                    object::pair{std:("service"), std:("service")}
                });
            }
            );
            if (elizaOSServicesPlugin->init) {
                std::async([=]() { elizaOSServicesPlugin->init(object{
                    object::pair{std:("ELIZAOS_API_KEY"), std:("test-value")}
                }, as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                if (elizaOSServicesPlugin->services) {
                    auto ElizaOSServiceClass = const_(elizaOSServicesPlugin->services)[0];
                    if (in(std:("start"), ElizaOSServiceClass)) {
                        auto serviceInstance = std::async([=]() { ElizaOSServiceClass->start(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                        mockRuntime->registerService(ElizaOSServiceClass);
                    }
                }
                expect(registerServiceCalls->get_length())->toBeGreaterThan(0);
            }
        }
        );
    }
    );
}

MAIN
