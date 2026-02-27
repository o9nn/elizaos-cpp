#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/plugin.test.h"

void documentTestResult(string testName, any result, any error)
{
    logger->info(std:("✓ Testing: ") + testName + string_empty);
    if (error) {
        logger->error(std:("✗ Error: ") + error->message + string_empty);
        if (error->stack) {
            logger->error(std:("Stack: ") + error->stack + string_empty);
        }
        return;
    }
    if (result) {
        if (type_of(result) == std:("string")) {
            if (AND((result->trim()), (result->get_length() > 0))) {
                auto preview = (result->get_length() > 60) ? string_empty + result->substring(0, 60) + std:("...") : result;
                logger->info(std:("  → ") + preview + string_empty);
            }
        } else if (type_of(result) == std:("object")) {
            try
            {
                auto keys = Object->keys(result);
                if (keys->get_length() > 0) {
                    auto preview = keys->slice(0, 3)->join(std:(", "));
                    auto more = (keys->get_length() > 3) ? any(std:(" +") + (keys->get_length() - 3) + std:(" more")) (string_empty);
                    logger->info(std:("  → {") + preview + string_empty + more + std:("}"));
                }
            }
            catch (const any& e)
            {
                logger->info(std:("  → [Complex object]"));
            }
        }
    }
};


any createRealRuntime()
{
    shared services = std::make_shared<Map>();
    shared createService = [=](auto serviceType) mutable
    {
        if (serviceType == StarterService::serviceType) {
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
        object::pair{std:("models"), plugin->models}, 
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
            if (!services->has(serviceType)) {
                services->set(serviceType, createService(serviceType));
            }
            return services->get(serviceType);
        }
        }, 
        object::pair{std:("registerService"), [=](auto serviceType, auto service) mutable
        {
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
            expect(plugin->name)->toBe(std:("starter"));
            expect(plugin->description)->toBe(std:("A starter plugin for Eliza"));
            expect(plugin->config)->toBeDefined();
            documentTestResult(std:("Plugin metadata check"), object{
                object::pair{std:("name"), plugin->name}, 
                object::pair{std:("description"), plugin->description}, 
                object::pair{std:("hasConfig"), !!plugin->config}
            });
        }
        );
        it(std:("should include the EXAMPLE_PLUGIN_VARIABLE in config"), [=]() mutable
        {
            expect(plugin->config)->toHaveProperty(std:("EXAMPLE_PLUGIN_VARIABLE"));
            documentTestResult(std:("Plugin config check"), object{
                object::pair{std:("hasExampleVariable"), (plugin->config) ? any(in(std:("EXAMPLE_PLUGIN_VARIABLE"), plugin->config)) (false)}, 
                object::pair{std:("configKeys"), Object->keys(OR((plugin->config), (object{})))}
            });
        }
        );
        it(std:("should initialize properly"), [=]() mutable
        {
            auto originalEnv = process->env->EXAMPLE_PLUGIN_VARIABLE;
            {
                utils::finally __finally4408_4473([&]() mutable
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = originalEnv;
                });
                try
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = std:("test-value");
                    auto runtime = createRealRuntime();
                    auto error = nullptr;
                    try
                    {
                        std::async([=]() { plugin->init(object{
                            object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), std:("test-value")}
                        }, as<any>(runtime)); });
                        expect(true)->toBe(true);
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std:("Plugin initialization error:"), e);
                    }
                    documentTestResult(std:("Plugin initialization"), object{
                        object::pair{std:("success"), !error}, 
                        object::pair{std:("configValue"), process->env->EXAMPLE_PLUGIN_VARIABLE}
                    }, error);
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
        it(std:("should throw an error on invalid config"), [=]() mutable
        {
            if (plugin->init) {
                auto runtime = createRealRuntime();
                auto error = nullptr;
                try
                {
                    std::async([=]() { plugin->init(object{
                        object::pair{std:("EXAMPLE_PLUGIN_VARIABLE"), string_empty}
                    }, as<any>(runtime)); });
                    expect(true)->toBe(false);
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                    expect(error)->toBeTruthy();
                }
                documentTestResult(std:("Plugin invalid config"), object{
                    object::pair{std:("errorThrown"), !!error}, 
                    object::pair{std:("errorMessage"), OR((error->message), (std:("No error message")))}
                }, error);
            }
        }
        );
        it(std:("should have a valid config"), [=]() mutable
        {
            expect(plugin->config)->toBeDefined();
            if (plugin->config) {
                expect(Object->keys(plugin->config))->toContain(std:("EXAMPLE_PLUGIN_VARIABLE"));
            }
        }
        );
    }
    );
    describe(std:("Plugin Models"), [=]() mutable
    {
        it(std:("should have TEXT_SMALL model defined"), [=]() mutable
        {
            if (plugin->models) {
                expect(plugin->models)->toHaveProperty(ModelType->TEXT_SMALL);
                expect(type_of(const_(plugin->models)[ModelType->TEXT_SMALL]))->toBe(std:("function"));
                documentTestResult(std:("TEXT_SMALL model check"), object{
                    object::pair{std:("defined"), in(ModelType->TEXT_SMALL, plugin->models)}, 
                    object::pair{std:("isFunction"), type_of(const_(plugin->models)[ModelType->TEXT_SMALL]) == std:("function")}
                });
            }
        }
        );
        it(std:("should have TEXT_LARGE model defined"), [=]() mutable
        {
            if (plugin->models) {
                expect(plugin->models)->toHaveProperty(ModelType->TEXT_LARGE);
                expect(type_of(const_(plugin->models)[ModelType->TEXT_LARGE]))->toBe(std:("function"));
                documentTestResult(std:("TEXT_LARGE model check"), object{
                    object::pair{std:("defined"), in(ModelType->TEXT_LARGE, plugin->models)}, 
                    object::pair{std:("isFunction"), type_of(const_(plugin->models)[ModelType->TEXT_LARGE]) == std:("function")}
                });
            }
        }
        );
        it(std:("should return a response from TEXT_SMALL model"), [=]() mutable
        {
            if (AND((plugin->models), (const_(plugin->models)[ModelType->TEXT_SMALL]))) {
                auto runtime = createRealRuntime();
                auto result = string_empty;
                auto error = nullptr;
                try
                {
                    logger->info(std:("Using OpenAI for TEXT_SMALL model"));
                    result = std::async([=]() { const_(plugin->models)[ModelType->TEXT_SMALL](as<any>(runtime), object{
                        object::pair{std:("prompt"), std:("test")}
                    }); });
                    expect(result)->toBeTruthy();
                    expect(type_of(result))->toBe(std:("string"));
                    expect(result->get_length())->toBeGreaterThan(10);
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                    logger->error(std:("TEXT_SMALL model test failed:"), e);
                }
                documentTestResult(std:("TEXT_SMALL model plugin test"), result, error);
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
            any startResult;
            auto error = nullptr;
            try
            {
                logger->info(std:("Using OpenAI for TEXT_SMALL model"));
                startResult = std::async([=]() { StarterService::start(as<any>(runtime)); });
                expect(startResult)->toBeDefined();
                expect(startResult["constructor"]["name"])->toBe(std:("StarterService"));
                expect(type_of(startResult["stop"]))->toBe(std:("function"));
            }
            catch (const any& e)
            {
                error = as<std::shared_ptr<Error>>(e);
                logger->error(std:("Service start error:"), e);
            }
            documentTestResult(std:("StarterService start"), object{
                object::pair{std:("success"), !!startResult}, 
                object::pair{std:("serviceType"), startResult["constructor"]["name"]}
            }, error);
        }
        );
        it(std:("should throw an error on startup if the service is already registered"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto result1 = std::async([=]() { StarterService::start(as<any>(runtime)); });
            expect(result1)->toBeTruthy();
            auto startupError = nullptr;
            try
            {
                std::async([=]() { StarterService::start(as<any>(runtime)); });
                expect(true)->toBe(false);
            }
            catch (const any& e)
            {
                startupError = as<std::shared_ptr<Error>>(e);
                expect(e)->toBeTruthy();
            }
            documentTestResult(std:("StarterService double start"), object{
                object::pair{std:("errorThrown"), !!startupError}, 
                object::pair{std:("errorMessage"), OR((startupError->message), (std:("No error message")))}
            }, startupError);
        }
        );
        it(std:("should stop the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto error = nullptr;
            try
            {
                auto service = std::make_shared<StarterService>(as<any>(runtime));
                runtime["registerService"](StarterService::serviceType, service);
                auto stopSpy = spyOn(service, std:("stop"));
                std::async([=]() { StarterService::stop(as<any>(runtime)); });
                expect(stopSpy)->toHaveBeenCalled();
            }
            catch (const any& e)
            {
                error = as<std::shared_ptr<Error>>(e);
                logger->error(std:("Service stop error:"), e);
            }
            documentTestResult(std:("StarterService stop"), object{
                object::pair{std:("success"), !error}
            }, error);
        }
        );
        it(std:("should throw an error when stopping a non-existent service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto error = nullptr;
            try
            {
                auto originalGetService = runtime["getService"];
                runtime["getService"] = [=]() mutable
                {
                    return nullptr;
                };
                std::async([=]() { StarterService::stop(as<any>(runtime)); });
                expect(true)->toBe(false);
            }
            catch (const any& e)
            {
                error = as<std::shared_ptr<Error>>(e);
                expect(error)->toBeTruthy();
                if (is<Error>(error)) {
                    expect(error->message)->toContain(std:("Starter service not found"));
                }
            }
            documentTestResult(std:("StarterService non-existent stop"), object{
                object::pair{std:("errorThrown"), !!error}, 
                object::pair{std:("errorMessage"), OR((error->message), (std:("No error message")))}
            }, error);
        }
        );
        it(std:("should stop a registered service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto startResult = std::async([=]() { StarterService::start(as<any>(runtime)); });
            expect(startResult)->toBeTruthy();
            auto stopError = nullptr;
            auto stopSuccess = false;
            try
            {
                std::async([=]() { StarterService::stop(as<any>(runtime)); });
                stopSuccess = true;
            }
            catch (const any& e)
            {
                stopError = e;
                expect(true)->toBe(false);
            }
            documentTestResult(std:("StarterService stop"), object{
                object::pair{std:("success"), stopSuccess}, 
                object::pair{std:("errorThrown"), !!stopError}, 
                object::pair{std:("errorMessage"), (is<Error>(stopError)) ? stopError->message : String(stopError)}
            }, (is<Error>(stopError)) ? any(stopError) (nullptr));
        }
        );
    }
    );
}

MAIN
