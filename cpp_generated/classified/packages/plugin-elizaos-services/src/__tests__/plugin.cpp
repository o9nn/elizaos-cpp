#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-elizaos-services/src/__tests__/plugin.test.h"

any createRealRuntime()
{
    shared services = std::make_shared<Map>();
    shared createService = [=](auto serviceType) mutable
    {
        if (serviceType == ElizaOSService::serviceType) {
            return std::make_shared<ElizaOSService>(as<any>(object{
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Character")}, 
                    object::pair{std::string("system"), std::string("You are a helpful assistant for testing.")}
                }}
            }));
        }
        return nullptr;
    };
    return object{
        object::pair{std::string("character"), object{
            object::pair{std::string("name"), std::string("Test Character")}, 
            object::pair{std::string("system"), std::string("You are a helpful assistant for testing.")}, 
            object::pair{std::string("plugins"), array<any>()}, 
            object::pair{std::string("settings"), object{}}
        }}, 
        object::pair{std::string("getSetting"), [=](auto key) mutable
        {
            return nullptr;
        }
        }, 
        object::pair{std::string("models"), elizaOSServicesPlugin->models}, 
        object::pair{std::string("db"), object{
            object::pair{std::string("get"), [=](auto key) mutable
            {
                return nullptr;
            }
            }, 
            object::pair{std::string("set"), [=](auto key, auto value) mutable
            {
                return true;
            }
            }, 
            object::pair{std::string("delete"), [=](auto key) mutable
            {
                return true;
            }
            }, 
            object::pair{std::string("getKeys"), [=](auto pattern) mutable
            {
                return array<any>();
            }
            }
        }}, 
        object::pair{std::string("getService"), [=](auto serviceType) mutable
        {
            logger->debug(std::string("Requesting service: ") + serviceType + string_empty);
            if (!services->has(serviceType)) {
                logger->debug(std::string("Creating new service: ") + serviceType + string_empty);
                services->set(serviceType, createService(serviceType));
            }
            return services->get(serviceType);
        }
        }, 
        object::pair{std::string("registerService"), [=](auto serviceType, auto service) mutable
        {
            logger->debug(std::string("Registering service: ") + serviceType + string_empty);
            services->set(serviceType, service);
        }
        }
    };
};



void Main(void)
{
    dotenv->config();
    beforeAll([=]() mutable
    {
        spyOn(logger, std::string("info"));
        spyOn(logger, std::string("error"));
        spyOn(logger, std::string("warn"));
        spyOn(logger, std::string("debug"));
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std::string("Plugin Configuration"), [=]() mutable
    {
        it(std::string("should have correct plugin metadata"), [=]() mutable
        {
            expect(elizaOSServicesPlugin->name)->toBe(std::string("elizaos-services"));
            expect(elizaOSServicesPlugin->description)->toBe(std::string("ElizaOS hosted services for AI inference and storage"));
            expect(elizaOSServicesPlugin->config)->toBeDefined();
        }
        );
        it(std::string("should include the EXAMPLE_PLUGIN_VARIABLE in config"), [=]() mutable
        {
            expect(elizaOSServicesPlugin->config)->toHaveProperty(std::string("ELIZAOS_API_KEY"));
        }
        );
        it(std::string("should initialize properly"), [=]() mutable
        {
            auto originalEnv = process->env->ELIZAOS_API_KEY;
            {
                utils::finally __finally2993_3050([&]() mutable
                {
                    process->env->ELIZAOS_API_KEY = originalEnv;
                });
                try
                {
                    process->env->ELIZAOS_API_KEY = std::string("test-value");
                    auto runtime = createRealRuntime();
                    if (elizaOSServicesPlugin->init) {
                        std::async([=]() { elizaOSServicesPlugin->init(object{
                            object::pair{std::string("ELIZAOS_API_KEY"), std::string("test-value")}
                        }, as<any>(runtime)); });
                        expect(true)->toBe(true);
                    }
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
        it(std::string("should have a valid config"), [=]() mutable
        {
            expect(elizaOSServicesPlugin->config)->toBeDefined();
            if (elizaOSServicesPlugin->config) {
                expect(Object->keys(elizaOSServicesPlugin->config))->toContain(std::string("ELIZAOS_API_KEY"));
            }
        }
        );
    }
    );
    describe(std::string("Plugin Models"), [=]() mutable
    {
        it(std::string("should have TEXT_SMALL model defined"), [=]() mutable
        {
            expect(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL])->toBeDefined();
            if (elizaOSServicesPlugin->models) {
                expect(type_of(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should have TEXT_LARGE model defined"), [=]() mutable
        {
            expect(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_LARGE])->toBeDefined();
            if (elizaOSServicesPlugin->models) {
                expect(type_of(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_LARGE]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should return a response from TEXT_SMALL model when API is available"), [=]() mutable
        {
            if (const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL]) {
                auto runtime = createRealRuntime();
                try
                {
                    auto result = std::async([=]() { const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL](as<any>(runtime), object{
                        object::pair{std::string("prompt"), std::string("test")}
                    }); });
                    expect(result)->toBeTruthy();
                    expect(type_of(result))->toBe(std::string("string"));
                    expect(result->length)->toBeGreaterThan(10);
                }
                catch (const any& error)
                {
                    if (AND((is<Error>(error)), ((OR((OR((OR((error->message->includes(std::string("No API provider available"))), (error->message->includes(std::string("API error"))))), (error->message->includes(std::string("invalid_api_key"))))), (error->message->includes(std::string("Incorrect API key")))))))) {
                        console->log(std::string("Skipping test - API provider not available or not configured in test environment"));
                        expect(error->message)->toBeTruthy();
                    } else {
                        throw any(error);
                    }
                }
            }
        }
        );
    }
    );
    describe(std::string("ElizaOSService"), [=]() mutable
    {
        it(std::string("should start the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto startResult = std::async([=]() { ElizaOSService::start(as<any>(runtime)); });
            expect(startResult)->toBeDefined();
            expect(startResult->constructor->name)->toBe(std::string("ElizaOSService"));
            expect(type_of(std::bind(&ElizaOSService::stop, startResult)))->toBe(std::string("function"));
        }
        );
        it(std::string("should stop the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto service = std::make_shared<ElizaOSService>(as<any>(runtime));
            runtime["registerService"](ElizaOSService::serviceType, service);
            auto stopSpy = spyOn(service, std::string("stop"));
            std::async([=]() { ElizaOSService::stop(as<any>(runtime)); });
            expect(stopSpy)->toHaveBeenCalled();
        }
        );
        it(std::string("should throw an error when stopping a non-existent service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto originalGetService = runtime["getService"];
            runtime["getService"] = [=]() mutable
            {
                return nullptr;
            };
            std::async([=]() { expect(ElizaOSService::stop(as<any>(runtime)))->rejects->toThrow(std::string("ElizaOS service not found")); });
            runtime["getService"] = originalGetService;
        }
        );
    }
    );
}

MAIN
