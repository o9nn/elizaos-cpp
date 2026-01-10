#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/plugin.test.h"

void documentTestResult(string testName, any result, any error)
{
    logger->info(std::string("✓ Testing: ") + testName + string_empty);
    if (error) {
        logger->error(std::string("✗ Error: ") + error->message + string_empty);
        if (error->stack) {
            logger->error(std::string("Stack: ") + error->stack + string_empty);
        }
        return;
    }
    if (result) {
        if (type_of(result) == std::string("string")) {
            if (AND((result->trim()), (result->get_length() > 0))) {
                auto preview = (result->get_length() > 60) ? string_empty + result->substring(0, 60) + std::string("...") : result;
                logger->info(std::string("  → ") + preview + string_empty);
            }
        } else if (type_of(result) == std::string("object")) {
            try
            {
                auto keys = Object->keys(result);
                if (keys->get_length() > 0) {
                    auto preview = keys->slice(0, 3)->join(std::string(", "));
                    auto more = (keys->get_length() > 3) ? any(std::string(" +") + (keys->get_length() - 3) + std::string(" more")) : any(string_empty);
                    logger->info(std::string("  → {") + preview + string_empty + more + std::string("}"));
                }
            }
            catch (const any& e)
            {
                logger->info(std::string("  → [Complex object]"));
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
        object::pair{std::string("models"), plugin->models}, 
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
            if (!services->has(serviceType)) {
                services->set(serviceType, createService(serviceType));
            }
            return services->get(serviceType);
        }
        }, 
        object::pair{std::string("registerService"), [=](auto serviceType, auto service) mutable
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
            expect(plugin->name)->toBe(std::string("starter"));
            expect(plugin->description)->toBe(std::string("A starter plugin for Eliza"));
            expect(plugin->config)->toBeDefined();
            documentTestResult(std::string("Plugin metadata check"), object{
                object::pair{std::string("name"), plugin->name}, 
                object::pair{std::string("description"), plugin->description}, 
                object::pair{std::string("hasConfig"), !!plugin->config}
            });
        }
        );
        it(std::string("should include the EXAMPLE_PLUGIN_VARIABLE in config"), [=]() mutable
        {
            expect(plugin->config)->toHaveProperty(std::string("EXAMPLE_PLUGIN_VARIABLE"));
            documentTestResult(std::string("Plugin config check"), object{
                object::pair{std::string("hasExampleVariable"), (plugin->config) ? any(in(std::string("EXAMPLE_PLUGIN_VARIABLE"), plugin->config)) : any(false)}, 
                object::pair{std::string("configKeys"), Object->keys(OR((plugin->config), (object{})))}
            });
        }
        );
        it(std::string("should initialize properly"), [=]() mutable
        {
            auto originalEnv = process->env->EXAMPLE_PLUGIN_VARIABLE;
            {
                utils::finally __finally4408_4473([&]() mutable
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = originalEnv;
                });
                try
                {
                    process->env->EXAMPLE_PLUGIN_VARIABLE = std::string("test-value");
                    auto runtime = createRealRuntime();
                    auto error = nullptr;
                    try
                    {
                        std::async([=]() { plugin->init(object{
                            object::pair{std::string("EXAMPLE_PLUGIN_VARIABLE"), std::string("test-value")}
                        }, as<any>(runtime)); });
                        expect(true)->toBe(true);
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std::string("Plugin initialization error:"), e);
                    }
                    documentTestResult(std::string("Plugin initialization"), object{
                        object::pair{std::string("success"), !error}, 
                        object::pair{std::string("configValue"), process->env->EXAMPLE_PLUGIN_VARIABLE}
                    }, error);
                }
                catch (...)
                {
                    throw;
                }
            }
        }
        );
        it(std::string("should throw an error on invalid config"), [=]() mutable
        {
            if (plugin->init) {
                auto runtime = createRealRuntime();
                auto error = nullptr;
                try
                {
                    std::async([=]() { plugin->init(object{
                        object::pair{std::string("EXAMPLE_PLUGIN_VARIABLE"), string_empty}
                    }, as<any>(runtime)); });
                    expect(true)->toBe(false);
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                    expect(error)->toBeTruthy();
                }
                documentTestResult(std::string("Plugin invalid config"), object{
                    object::pair{std::string("errorThrown"), !!error}, 
                    object::pair{std::string("errorMessage"), OR((error->message), (std::string("No error message")))}
                }, error);
            }
        }
        );
        it(std::string("should have a valid config"), [=]() mutable
        {
            expect(plugin->config)->toBeDefined();
            if (plugin->config) {
                expect(Object->keys(plugin->config))->toContain(std::string("EXAMPLE_PLUGIN_VARIABLE"));
            }
        }
        );
    }
    );
    describe(std::string("Plugin Models"), [=]() mutable
    {
        it(std::string("should have TEXT_SMALL model defined"), [=]() mutable
        {
            if (plugin->models) {
                expect(plugin->models)->toHaveProperty(ModelType->TEXT_SMALL);
                expect(type_of(const_(plugin->models)[ModelType->TEXT_SMALL]))->toBe(std::string("function"));
                documentTestResult(std::string("TEXT_SMALL model check"), object{
                    object::pair{std::string("defined"), in(ModelType->TEXT_SMALL, plugin->models)}, 
                    object::pair{std::string("isFunction"), type_of(const_(plugin->models)[ModelType->TEXT_SMALL]) == std::string("function")}
                });
            }
        }
        );
        it(std::string("should have TEXT_LARGE model defined"), [=]() mutable
        {
            if (plugin->models) {
                expect(plugin->models)->toHaveProperty(ModelType->TEXT_LARGE);
                expect(type_of(const_(plugin->models)[ModelType->TEXT_LARGE]))->toBe(std::string("function"));
                documentTestResult(std::string("TEXT_LARGE model check"), object{
                    object::pair{std::string("defined"), in(ModelType->TEXT_LARGE, plugin->models)}, 
                    object::pair{std::string("isFunction"), type_of(const_(plugin->models)[ModelType->TEXT_LARGE]) == std::string("function")}
                });
            }
        }
        );
        it(std::string("should return a response from TEXT_SMALL model"), [=]() mutable
        {
            if (AND((plugin->models), (const_(plugin->models)[ModelType->TEXT_SMALL]))) {
                auto runtime = createRealRuntime();
                auto result = string_empty;
                auto error = nullptr;
                try
                {
                    logger->info(std::string("Using OpenAI for TEXT_SMALL model"));
                    result = std::async([=]() { const_(plugin->models)[ModelType->TEXT_SMALL](as<any>(runtime), object{
                        object::pair{std::string("prompt"), std::string("test")}
                    }); });
                    expect(result)->toBeTruthy();
                    expect(type_of(result))->toBe(std::string("string"));
                    expect(result->get_length())->toBeGreaterThan(10);
                }
                catch (const any& e)
                {
                    error = as<std::shared_ptr<Error>>(e);
                    logger->error(std::string("TEXT_SMALL model test failed:"), e);
                }
                documentTestResult(std::string("TEXT_SMALL model plugin test"), result, error);
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
            any startResult;
            auto error = nullptr;
            try
            {
                logger->info(std::string("Using OpenAI for TEXT_SMALL model"));
                startResult = std::async([=]() { StarterService::start(as<any>(runtime)); });
                expect(startResult)->toBeDefined();
                expect(startResult["constructor"]["name"])->toBe(std::string("StarterService"));
                expect(type_of(startResult["stop"]))->toBe(std::string("function"));
            }
            catch (const any& e)
            {
                error = as<std::shared_ptr<Error>>(e);
                logger->error(std::string("Service start error:"), e);
            }
            documentTestResult(std::string("StarterService start"), object{
                object::pair{std::string("success"), !!startResult}, 
                object::pair{std::string("serviceType"), startResult["constructor"]["name"]}
            }, error);
        }
        );
        it(std::string("should throw an error on startup if the service is already registered"), [=]() mutable
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
            documentTestResult(std::string("StarterService double start"), object{
                object::pair{std::string("errorThrown"), !!startupError}, 
                object::pair{std::string("errorMessage"), OR((startupError->message), (std::string("No error message")))}
            }, startupError);
        }
        );
        it(std::string("should stop the service"), [=]() mutable
        {
            auto runtime = createRealRuntime();
            auto error = nullptr;
            try
            {
                auto service = std::make_shared<StarterService>(as<any>(runtime));
                runtime["registerService"](StarterService::serviceType, service);
                auto stopSpy = spyOn(service, std::string("stop"));
                std::async([=]() { StarterService::stop(as<any>(runtime)); });
                expect(stopSpy)->toHaveBeenCalled();
            }
            catch (const any& e)
            {
                error = as<std::shared_ptr<Error>>(e);
                logger->error(std::string("Service stop error:"), e);
            }
            documentTestResult(std::string("StarterService stop"), object{
                object::pair{std::string("success"), !error}
            }, error);
        }
        );
        it(std::string("should throw an error when stopping a non-existent service"), [=]() mutable
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
                    expect(error->message)->toContain(std::string("Starter service not found"));
                }
            }
            documentTestResult(std::string("StarterService non-existent stop"), object{
                object::pair{std::string("errorThrown"), !!error}, 
                object::pair{std::string("errorMessage"), OR((error->message), (std::string("No error message")))}
            }, error);
        }
        );
        it(std::string("should stop a registered service"), [=]() mutable
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
            documentTestResult(std::string("StarterService stop"), object{
                object::pair{std::string("success"), stopSuccess}, 
                object::pair{std::string("errorThrown"), !!stopError}, 
                object::pair{std::string("errorMessage"), (is<Error>(stopError)) ? stopError->message : String(stopError)}
            }, (is<Error>(stopError)) ? any(stopError) : any(nullptr));
        }
        );
    }
    );
}

MAIN
