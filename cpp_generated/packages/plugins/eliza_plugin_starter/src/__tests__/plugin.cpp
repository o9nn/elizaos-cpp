#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-starter/src/__tests__/plugin.test.h"

any createRealRuntime()
{
    shared services = std::make_shared<Map>();
    shared createService = [=](auto serviceType) mutable
    {
        if (serviceType == StarterService->serviceType) {
            return std::make_shared<StarterService>(as<any>(object{
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
        object::pair{std::string("models"), starterPlugin->models}, 
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
            expect(starterPlugin->name)->toBe(std::string("plugin-starter"));
            expect(starterPlugin->description)->toBe(std::string("Plugin starter for elizaOS"));
            expect(starterPlugin->config)->toBeDefined();
        }
        );
        it(std::string("should include the EXAMPLE_PLUGIN_VARIABLE in config"), [=]() mutable
        {
            expect(starterPlugin->config)->toHaveProperty(std::string("EXAMPLE_PLUGIN_VARIABLE"));
        }
        );
        it(std::string("should initialize properly"), [=]() mutable
        {
            auto originalEnv = process->env->EXAMPLE_PLUGIN_VARIABLE;
            {
                utils::finally __finally2916_2981([&]() mutable
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = originalEnv;
                });
                try
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = std::string("test-value");
                    auto runtime = createRealRuntime();
                    if (starterPlugin->init) {
                        std::async([=]() { starterPlugin->init(object{
                            object::pair{std::string("EXAMPLE_PLUGIN_VARIABLE"), std::string("test-value")}
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
            expect(starterPlugin->config)->toBeDefined();
            if (starterPlugin->config) {
                expect(Object->keys(starterPlugin->config))->toContain(std::string("EXAMPLE_PLUGIN_VARIABLE"));
            }
        }
        );
    }
    );
    describe(std::string("Plugin Models"), [=]() mutable
    {
        it(std::string("should have TEXT_SMALL model defined"), [=]() mutable
        {
            expect(const_(starterPlugin->models)[ModelType->TEXT_SMALL])->toBeDefined();
            if (starterPlugin->models) {
                expect(type_of(const_(starterPlugin->models)[ModelType->TEXT_SMALL]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should have TEXT_LARGE model defined"), [=]() mutable
        {
            expect(const_(starterPlugin->models)[ModelType->TEXT_LARGE])->toBeDefined();
            if (starterPlugin->models) {
                expect(type_of(const_(starterPlugin->models)[ModelType->TEXT_LARGE]))->toBe(std::string("function"));
            }
        }
        );
        it(std::string("should return a response from TEXT_SMALL model"), [=]() mutable
        {
            if (const_(starterPlugin->models)[ModelType->TEXT_SMALL]) {
                auto runtime = createRealRuntime();
                auto result = std::async([=]() { const_(starterPlugin->models)[ModelType->TEXT_SMALL](as<any>(runtime), object{
                    object::pair{std::string("prompt"), std::string("test")}
                }); });
                expect(result)->toBeTruthy();
                expect(type_of(result))->toBe(std::string("string"));
                expect(result->length)->toBeGreaterThan(10);
            }
        }
        );
    }
    );
    describe(std::string("StarterService"), [=]() mutable
    {
        it(std::string("should start the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto startResult = std::async([=]() { StarterService->start(as<any>(runtime)); });
            expect(startResult)->toBeDefined();
            expect(startResult->constructor->name)->toBe(std::string("StarterService"));
            expect(type_of(startResult->stop))->toBe(std::string("function"));
        }
        );
        it(std::string("should stop the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto service = std::make_shared<StarterService>(as<any>(runtime));
            runtime["registerService"](StarterService->serviceType, service);
            auto stopSpy = spyOn(service, std::string("stop"));
            std::async([=]() { StarterService->stop(as<any>(runtime)); });
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
            std::async([=]() { expect(StarterService->stop(as<any>(runtime)))->rejects->toThrow(std::string("Starter service not found")); });
            runtime["getService"] = originalGetService;
        }
        );
    }
    );
}

MAIN
