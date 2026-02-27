#include "plugin.test.h"

any createRealRuntime()
{
    shared services = std::make_shared<Map>();
    shared createService = [=](auto serviceType) mutable
    {
        if (serviceType == ElizaOSService::serviceType) {
            return std::make_shared<ElizaOSService>(as<any>(object{
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Character")}, 
                    object::pair{std:("system"), std:("You are a helpful assistant for testing.")}
                }}
            }));
        }
        return nullptr;
    };
    return object{
        object::pair{std:("character"), object{
            object::pair{std:("name"), std:("Test Character")}, 
            object::pair{std:("system"), std:("You are a helpful assistant for testing.")}, 
            object::pair{std:("plugins"), array<any>()}, 
            object::pair{std:("settings"), object{}}
        }}, 
        object::pair{std:("getSetting"), [=](auto key) mutable
        {
            return nullptr;
        }
        }, 
        object::pair{std:("models"), elizaOSServicesPlugin->models}, 
        object::pair{std:("db"), object{
            object::pair{std:("get"), [=](auto key) mutable
            {
                return nullptr;
            }
            }, 
            object::pair{std:("set"), [=](auto key, auto value) mutable
            {
                return true;
            }
            }, 
            object::pair{std:("delete"), [=](auto key) mutable
            {
                return true;
            }
            }, 
            object::pair{std:("getKeys"), [=](auto pattern) mutable
            {
                return array<any>();
            }
            }
        }}, 
        object::pair{std:("getService"), [=](auto serviceType) mutable
        {
            logger->debug(std:("Requesting service: ") + serviceType + string_empty);
            if (!services->has(serviceType)) {
                logger->debug(std:("Creating new service: ") + serviceType + string_empty);
                services->set(serviceType, createService(serviceType));
            }
            return services->get(serviceType);
        }
        }, 
        object::pair{std:("registerService"), [=](auto serviceType, auto service) mutable
        {
            logger->debug(std:("Registering service: ") + serviceType + string_empty);
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
        spyOn(logger, std:("info"));
        spyOn(logger, std:("error"));
        spyOn(logger, std:("warn"));
        spyOn(logger, std:("debug"));
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std:("Plugin Configuration"), [=]() mutable
    {
        it(std:("should have correct plugin metadata"), [=]() mutable
        {
            expect(elizaOSServicesPlugin->name)->toBe(std:("elizaos-services"));
            expect(elizaOSServicesPlugin->description)->toBe(std:("ElizaOS hosted services for AI inference and storage"));
            expect(elizaOSServicesPlugin->config)->toBeDefined();
        }
        );
        it(std:("should include the EXAMPLE_PLUGIN_VARIABLE in config"), [=]() mutable
        {
            expect(elizaOSServicesPlugin->config)->toHaveProperty(std:("ELIZAOS_API_KEY"));
        }
        );
        it(std:("should initialize properly"), [=]() mutable
        {
            auto originalEnv = process->env->ELIZAOS_API_KEY;
            {
                utils::finally __finally2993_3050([&]() mutable
                {
                    process->env->ELIZAOS_API_KEY = originalEnv;
                });
                try
                {
                    process->env->ELIZAOS_API_KEY = std:("test-value");
                    auto runtime = createRealRuntime();
                    if (elizaOSServicesPlugin->init) {
                        std::async([=]() { elizaOSServicesPlugin->init(object{
                            object::pair{std:("ELIZAOS_API_KEY"), std:("test-value")}
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
        it(std:("should have a valid config"), [=]() mutable
        {
            expect(elizaOSServicesPlugin->config)->toBeDefined();
            if (elizaOSServicesPlugin->config) {
                expect(Object->keys(elizaOSServicesPlugin->config))->toContain(std:("ELIZAOS_API_KEY"));
            }
        }
        );
    }
    );
    describe(std:("Plugin Models"), [=]() mutable
    {
        it(std:("should have TEXT_SMALL model defined"), [=]() mutable
        {
            expect(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL])->toBeDefined();
            if (elizaOSServicesPlugin->models) {
                expect(type_of(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should have TEXT_LARGE model defined"), [=]() mutable
        {
            expect(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_LARGE])->toBeDefined();
            if (elizaOSServicesPlugin->models) {
                expect(type_of(const_(elizaOSServicesPlugin->models)[ModelType->TEXT_LARGE]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should return a response from TEXT_SMALL model when API is available"), [=]() mutable
        {
            if (const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL]) {
                auto runtime = createRealRuntime();
                try
                {
                    auto result = std::async([=]() { const_(elizaOSServicesPlugin->models)[ModelType->TEXT_SMALL](as<any>(runtime), object{
                        object::pair{std:("prompt"), std:("test")}
                    }); });
                    expect(result)->toBeTruthy();
                    expect(type_of(result))->toBe(std:("string"));
                    expect(result->length)->toBeGreaterThan(10);
                }
                catch (const any& error)
                {
                    if (AND((is<Error>(error)), ((OR((OR((OR((error->message->includes(std:("No API provider available"))), (error->message->includes(std:("API error"))))), (error->message->includes(std:("invalid_api_key"))))), (error->message->includes(std:("Incorrect API key")))))))) {
                        console->log(std:("Skipping test - API provider not available or not configured in test environment"));
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
    describe(std:("ElizaOSService"), [=]() mutable
    {
        it(std:("should start the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto startResult = std::async([=]() { ElizaOSService::start(as<any>(runtime)); });
            expect(startResult)->toBeDefined();
            expect(startResult->constructor->name)->toBe(std:("ElizaOSService"));
            expect(type_of(std::bind(&ElizaOSService::stop, startResult)))->toBe(std:("function"));
        }
        );
        it(std:("should stop the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto service = std::make_shared<ElizaOSService>(as<any>(runtime));
            runtime["registerService"](ElizaOSService::serviceType, service);
            auto stopSpy = spyOn(service, std:("stop"));
            std::async([=]() { ElizaOSService::stop(as<any>(runtime)); });
            expect(stopSpy)->toHaveBeenCalled();
        }
        );
        it(std:("should throw an error when stopping a non-existent service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto originalGetService = runtime["getService"];
            runtime["getService"] = [=]() mutable
            {
                return nullptr;
            };
            std::async([=]() { expect(ElizaOSService::stop(as<any>(runtime)))->rejects->toThrow(std:("ElizaOS service not found")); });
            runtime["getService"] = originalGetService;
        }
        );
    }
    );
}

MAIN
