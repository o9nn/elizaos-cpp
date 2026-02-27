#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-starter/src/__tests__/plugin.test.h"

any createRealRuntime()
{
    shared services = std::make_shared<Map>();
    shared createService = [=](auto serviceType) mutable
    {
        if (serviceType == StarterService->serviceType) {
            return std::make_shared<StarterService>(as<any>(object{
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
        object::pair{std:("models"), starterPlugin->models}, 
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
            expect(starterPlugin->name)->toBe(std:("plugin-starter"));
            expect(starterPlugin->description)->toBe(std:("Plugin starter for elizaOS"));
            expect(starterPlugin->config)->toBeDefined();
        }
        );
        it(std:("should include the EXAMPLE_PLUGIN_VARIABLE in config"), [=]() mutable
        {
            expect(starterPlugin->config)->toHaveProperty(std:("EXAMPLE_PLUGIN_VARIABLE"));
        }
        );
        it(std:("should initialize properly"), [=]() mutable
        {
            auto originalEnv = process->env->EXAMPLE_PLUGIN_VARIABLE;
            {
                utils::finally __finally2916_2981([&]() mutable
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = originalEnv;
                });
                try
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = std:("test-value");
                    auto runtime = createRealRuntime();
                    if (starterPlugin->init) {
                        std::async([=]() { starterPlugin->init(object{
                            object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), std:("test-value")}
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
            expect(starterPlugin->config)->toBeDefined();
            if (starterPlugin->config) {
                expect(Object->keys(starterPlugin->config))->toContain(std:("EXAMPLE_PLUGIN_VARIABLE"));
            }
        }
        );
    }
    );
    describe(std:("Plugin Models"), [=]() mutable
    {
        it(std:("should have TEXT_SMALL model defined"), [=]() mutable
        {
            expect(const_(starterPlugin->models)[ModelType->TEXT_SMALL])->toBeDefined();
            if (starterPlugin->models) {
                expect(type_of(const_(starterPlugin->models)[ModelType->TEXT_SMALL]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should have TEXT_LARGE model defined"), [=]() mutable
        {
            expect(const_(starterPlugin->models)[ModelType->TEXT_LARGE])->toBeDefined();
            if (starterPlugin->models) {
                expect(type_of(const_(starterPlugin->models)[ModelType->TEXT_LARGE]))->toBe(std:("function"));
            }
        }
        );
        it(std:("should return a response from TEXT_SMALL model"), [=]() mutable
        {
            if (const_(starterPlugin->models)[ModelType->TEXT_SMALL]) {
                auto runtime = createRealRuntime();
                auto result = std::async([=]() { const_(starterPlugin->models)[ModelType->TEXT_SMALL](as<any>(runtime), object{
                    object::pair{std:("prompt"), std:("test")}
                }); });
                expect(result)->toBeTruthy();
                expect(type_of(result))->toBe(std:("string"));
                expect(result->length)->toBeGreaterThan(10);
            }
        }
        );
    }
    );
    describe(std:("StarterService"), [=]() mutable
    {
        it(std:("should start the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto startResult = std::async([=]() { StarterService->start(as<any>(runtime)); });
            expect(startResult)->toBeDefined();
            expect(startResult->constructor->name)->toBe(std:("StarterService"));
            expect(type_of(startResult->stop))->toBe(std:("function"));
        }
        );
        it(std:("should stop the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto service = std::make_shared<StarterService>(as<any>(runtime));
            runtime["registerService"](StarterService->serviceType, service);
            auto stopSpy = spyOn(service, std:("stop"));
            std::async([=]() { StarterService->stop(as<any>(runtime)); });
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
            std::async([=]() { expect(StarterService->stop(as<any>(runtime)))->rejects->toThrow(std:("Starter service not found")); });
            runtime["getService"] = originalGetService;
        }
        );
    }
    );
}

MAIN
