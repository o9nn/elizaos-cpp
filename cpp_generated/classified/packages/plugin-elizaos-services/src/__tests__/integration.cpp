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
    describe(std::string("Integration: ElizaOS Models with ElizaOSService"), [=]() mutable
    {
        shared<std::shared_ptr<MockRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            shared mockService = object{
                object::pair{std::string("capabilityDescription"), std::string("ElizaOS hosted AI inference and storage services with multi-provider support")}, 
                object::pair{std::string("stop"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }
            };
            auto getServiceImpl = [=](auto serviceType) mutable
            {
                if (serviceType == std::string("elizaos-services")) {
                    return mockService;
                }
                return nullptr;
            };
            mockRuntime = createMockRuntime(object{
                object::pair{std::string("getService"), mock()->mockImplementation(getServiceImpl)}
            });
        }
        );
        it(std::string("should handle TEXT_SMALL model with ElizaOSService available"), [=]() mutable
        {
            auto textSmallModel = elizaOSServicesPlugin->models->TEXT_SMALL;
            expect(textSmallModel)->toBeDefined();
            try
            {
                std::async([=]() { textSmallModel(as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime)), object{
                    object::pair{std::string("prompt"), std::string("test prompt")}, 
                    object::pair{std::string("maxTokens"), 10}, 
                    object::pair{std::string("temperature"), 0.5}
                }); });
            }
            catch (const any& error)
            {
                expect((as<std::shared_ptr<Error>>(error))->message)->toContain(std::string("No API provider available"));
            }
            auto service = mockRuntime->getService(std::string("elizaos-services"));
            expect(service)->toBeDefined();
            expect(service["capabilityDescription"])->toContain(std::string("ElizaOS"));
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
            mockRuntime->registerService = mock()->mockImplementation([=](auto type, auto service) mutable
            {
                registerServiceCalls->push(object{
                    object::pair{std::string("type"), std::string("type")}, 
                    object::pair{std::string("service"), std::string("service")}
                });
            }
            );
            if (elizaOSServicesPlugin->init) {
                std::async([=]() { elizaOSServicesPlugin->init(object{
                    object::pair{std::string("ELIZAOS_API_KEY"), std::string("test-value")}
                }, as<std::shared_ptr<IAgentRuntime>>(as<any>(mockRuntime))); });
                if (elizaOSServicesPlugin->services) {
                    auto ElizaOSServiceClass = const_(elizaOSServicesPlugin->services)[0];
                    if (in(std::string("start"), ElizaOSServiceClass)) {
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
