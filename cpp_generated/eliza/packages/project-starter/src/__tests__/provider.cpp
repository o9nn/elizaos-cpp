#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/provider.test.h"

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


std::shared_ptr<IAgentRuntime> createRealRuntime()
{
    return as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
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
        object::pair{std::string("memory"), object{
            object::pair{std::string("add"), [=](auto memory) mutable
            {
            }
            }, 
            object::pair{std::string("get"), [=](auto id) mutable
            {
                return nullptr;
            }
            }, 
            object::pair{std::string("getByEntityId"), [=](auto entityId) mutable
            {
                return array<any>();
            }
            }, 
            object::pair{std::string("getLatest"), [=](auto entityId) mutable
            {
                return nullptr;
            }
            }, 
            object::pair{std::string("getRecentMessages"), [=](auto options) mutable
            {
                return array<any>();
            }
            }, 
            object::pair{std::string("search"), [=](auto query) mutable
            {
                return array<any>();
            }
            }
        }}, 
        object::pair{std::string("getService"), [=](auto serviceType) mutable
        {
            return nullptr;
        }
        }
    }));
};


std::shared_ptr<Memory> createRealMemory()
{
    auto entityId = uuidv4();
    auto roomId = uuidv4();
    return as<std::shared_ptr<Memory>>(object{
        object::pair{std::string("id"), uuidv4()}, 
        object::pair{std::string("entityId"), std::string("entityId")}, 
        object::pair{std::string("roomId"), std::string("roomId")}, 
        object::pair{std::string("timestamp"), Date->now()}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("What can you provide?")}, 
            object::pair{std::string("source"), std::string("test")}, 
            object::pair{std::string("actions"), array<any>()}
        }}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("type"), std::string("custom")}, 
            object::pair{std::string("sessionId"), uuidv4()}, 
            object::pair{std::string("conversationId"), uuidv4()}
        }}
    });
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
    describe(std::string("Provider Tests"), [=]() mutable
    {
        shared helloWorldProvider = plugin->providers->find([=](auto provider) mutable
        {
            return provider["name"] == std::string("HELLO_WORLD_PROVIDER");
        }
        );
        describe(std::string("HELLO_WORLD_PROVIDER"), [=]() mutable
        {
            it(std::string("should exist in the plugin"), [=]() mutable
            {
                expect(plugin->providers)->toBeDefined();
                expect(Array->isArray(plugin->providers))->toBe(true);
                if (plugin->providers) {
                    expect(plugin->providers->length)->toBeGreaterThan(0);
                    auto result = plugin->providers->find([=](auto p) mutable
                    {
                        return p["name"] == std::string("HELLO_WORLD_PROVIDER");
                    }
                    );
                    expect(result)->toBeDefined();
                    documentTestResult(std::string("Provider exists check"), object{
                        object::pair{std::string("found"), !!result}, 
                        object::pair{std::string("providers"), plugin->providers->map([=](auto p) mutable
                        {
                            return p["name"];
                        }
                        )}
                    });
                }
            }
            );
            it(std::string("should have the correct structure"), [=]() mutable
            {
                if (helloWorldProvider) {
                    expect(helloWorldProvider)->toHaveProperty(std::string("name"), std::string("HELLO_WORLD_PROVIDER"));
                    expect(helloWorldProvider)->toHaveProperty(std::string("description"));
                    expect(helloWorldProvider)->toHaveProperty(std::string("get"));
                    expect(type_of(helloWorldProvider->get))->toBe(std::string("function"));
                    documentTestResult(std::string("Provider structure check"), object{
                        object::pair{std::string("name"), helloWorldProvider->name}, 
                        object::pair{std::string("description"), helloWorldProvider->description}, 
                        object::pair{std::string("hasGetMethod"), type_of(helloWorldProvider->get) == std::string("function")}
                    });
                }
            }
            );
            it(std::string("should have a description explaining its purpose"), [=]() mutable
            {
                if (AND((helloWorldProvider), (helloWorldProvider->description))) {
                    expect(type_of(helloWorldProvider->description))->toBe(std::string("string"));
                    expect(helloWorldProvider->description->length)->toBeGreaterThan(0);
                    documentTestResult(std::string("Provider description check"), object{
                        object::pair{std::string("description"), helloWorldProvider->description}
                    });
                }
            }
            );
            it(std::string("should return provider data from the get method"), [=]() mutable
            {
                if (helloWorldProvider) {
                    auto runtime = createRealRuntime();
                    auto message = createRealMemory();
                    auto state = as<std::shared_ptr<State>>(object{
                        object::pair{std::string("values"), object{
                            object::pair{std::string("example"), std::string("test value")}
                        }}, 
                        object::pair{std::string("data"), object{
                            object::pair{std::string("additionalContext"), std::string("some context")}
                        }}, 
                        object::pair{std::string("text"), std::string("Current state context")}
                    });
                    auto result = nullptr;
                    auto error = nullptr;
                    try
                    {
                        logger->info(std::string("Calling provider.get with real implementation"));
                        result = std::async([=]() { helloWorldProvider->get(runtime, message, state); });
                        expect(result)->toBeDefined();
                        expect(result)->toHaveProperty(std::string("text"));
                        expect(result)->toHaveProperty(std::string("values"));
                        expect(result)->toHaveProperty(std::string("data"));
                        if (AND((result), ((OR((!result["text"]), (result["text"]["length"] == 0)))))) {
                            logger->warn(std::string("Provider returned empty text"));
                        }
                        if (AND((result), (Object->keys(result["values"])->get_length() == 0))) {
                            logger->warn(std::string("Provider returned empty values object"));
                        }
                        if (AND((result), (Object->keys(result["data"])->get_length() == 0))) {
                            logger->warn(std::string("Provider returned empty data object"));
                        }
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std::string("Error in provider.get:"), e);
                    }
                    documentTestResult(std::string("Provider get method"), result, error);
                }
            }
            );
            it(std::string("should handle error conditions gracefully"), [=]() mutable
            {
                if (helloWorldProvider) {
                    auto runtime = createRealRuntime();
                    auto invalidMemory = as<std::shared_ptr<Memory>>(as<any>(object{
                        object::pair{std::string("id"), uuidv4()}
                    }));
                    auto state = as<std::shared_ptr<State>>(object{
                        object::pair{std::string("values"), object{}}, 
                        object::pair{std::string("data"), object{}}, 
                        object::pair{std::string("text"), string_empty}
                    });
                    auto result = nullptr;
                    auto error = nullptr;
                    try
                    {
                        logger->info(std::string("Calling provider.get with invalid memory object"));
                        result = std::async([=]() { helloWorldProvider->get(runtime, invalidMemory, state); });
                        expect(result)->toBeDefined();
                        logger->info(std::string("Provider handled invalid input without throwing"));
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std::string("Provider threw an error with invalid input:"), e);
                    }
                    documentTestResult(std::string("Provider error handling"), result, error);
                }
            }
            );
        }
        );
        describe(std::string("Provider Registration"), [=]() mutable
        {
            it(std::string("should include providers in the plugin definition"), [=]() mutable
            {
                expect(plugin)->toHaveProperty(std::string("providers"));
                expect(Array->isArray(plugin->providers))->toBe(true);
                documentTestResult(std::string("Plugin providers check"), object{
                    object::pair{std::string("hasProviders"), !!plugin->providers}, 
                    object::pair{std::string("providersCount"), OR((plugin->providers->length), (0))}
                });
            }
            );
            it(std::string("should correctly initialize providers array"), [=]() mutable
            {
                if (plugin->providers) {
                    expect(plugin->providers->length)->toBeGreaterThan(0);
                    shared allValid = true;
                    shared invalidProviders = array<string>();
                    plugin->providers->forEach([=](auto provider) mutable
                    {
                        auto isValid = AND((AND((provider->name != undefined), (provider->description != undefined))), (type_of(provider->get) == std::string("function")));
                        if (!isValid) {
                            allValid = false;
                            invalidProviders->push(OR((provider->name), (std::string("unnamed"))));
                        }
                        expect(provider)->toHaveProperty(std::string("name"));
                        expect(provider)->toHaveProperty(std::string("description"));
                        expect(provider)->toHaveProperty(std::string("get"));
                        expect(type_of(provider->get))->toBe(std::string("function"));
                    }
                    );
                    documentTestResult(std::string("Provider initialization check"), object{
                        object::pair{std::string("providersCount"), plugin->providers->length}, 
                        object::pair{std::string("allValid"), std::string("allValid")}, 
                        object::pair{std::string("invalidProviders"), std::string("invalidProviders")}
                    });
                }
            }
            );
            it(std::string("should have unique provider names"), [=]() mutable
            {
                if (plugin->providers) {
                    shared providerNames = plugin->providers->map([=](auto provider) mutable
                    {
                        return provider["name"];
                    }
                    );
                    auto uniqueNames = std::make_shared<Set>(providerNames);
                    auto duplicates = providerNames->filter([=](auto name, auto index) mutable
                    {
                        return providerNames->indexOf(name) != index;
                    }
                    );
                    expect(providerNames->length)->toBe(uniqueNames->size);
                    documentTestResult(std::string("Provider uniqueness check"), object{
                        object::pair{std::string("totalProviders"), providerNames->length}, 
                        object::pair{std::string("uniqueProviders"), uniqueNames->size}, 
                        object::pair{std::string("duplicates"), std::string("duplicates")}
                    });
                }
            }
            );
        }
        );
    }
    );
}

MAIN
