#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/provider.test.h"

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


std::shared_ptr<IAgentRuntime> createRealRuntime()
{
    return as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
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
        object::pair{std:("memory"), object{
            object::pair{std:("add"), [=](auto memory) mutable
            {
            }
            }, 
            object::pair{std:("get"), [=](auto id) mutable
            {
                return nullptr;
            }
            }, 
            object::pair{std:("getByEntityId"), [=](auto entityId) mutable
            {
                return array<any>();
            }
            }, 
            object::pair{std:("getLatest"), [=](auto entityId) mutable
            {
                return nullptr;
            }
            }, 
            object::pair{std:("getRecentMessages"), [=](auto options) mutable
            {
                return array<any>();
            }
            }, 
            object::pair{std:("search"), [=](auto query) mutable
            {
                return array<any>();
            }
            }
        }}, 
        object::pair{std:("getService"), [=](auto serviceType) mutable
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
        object::pair{std:("id"), uuidv4()}, 
        object::pair{std:("entityId"), std:("entityId")}, 
        object::pair{std:("roomId"), std:("roomId")}, 
        object::pair{std:("timestamp"), Date->now()}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("What can you provide?")}, 
            object::pair{std:("source"), std:("test")}, 
            object::pair{std:("actions"), array<any>()}
        }}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("type"), std:("custom")}, 
            object::pair{std:("sessionId"), uuidv4()}, 
            object::pair{std:("conversationId"), uuidv4()}
        }}
    });
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
    describe(std:("Provider Tests"), [=]() mutable
    {
        shared helloWorldProvider = plugin->providers->find([=](auto provider) mutable
        {
            return provider["name"] == std:("HELLO_WORLD_PROVIDER");
        }
        );
        describe(std:("HELLO_WORLD_PROVIDER"), [=]() mutable
        {
            it(std:("should exist in the plugin"), [=]() mutable
            {
                expect(plugin->providers)->toBeDefined();
                expect(Array->isArray(plugin->providers))->toBe(true);
                if (plugin->providers) {
                    expect(plugin->providers->length)->toBeGreaterThan(0);
                    auto result = plugin->providers->find([=](auto p) mutable
                    {
                        return p["name"] == std:("HELLO_WORLD_PROVIDER");
                    }
                    );
                    expect(result)->toBeDefined();
                    documentTestResult(std:("Provider exists check"), object{
                        object::pair{std:("found"), !!result}, 
                        object::pair{std:("providers"), plugin->providers->map([=](auto p) mutable
                        {
                            return p["name"];
                        }
                        )}
                    });
                }
            }
            );
            it(std:("should have the correct structure"), [=]() mutable
            {
                if (helloWorldProvider) {
                    expect(helloWorldProvider)->toHaveProperty(std:("name"), std:("HELLO_WORLD_PROVIDER"));
                    expect(helloWorldProvider)->toHaveProperty(std:("description"));
                    expect(helloWorldProvider)->toHaveProperty(std:("get"));
                    expect(type_of(helloWorldProvider->get))->toBe(std:("function"));
                    documentTestResult(std:("Provider structure check"), object{
                        object::pair{std:("name"), helloWorldProvider->name}, 
                        object::pair{std:("description"), helloWorldProvider->description}, 
                        object::pair{std:("hasGetMethod"), type_of(helloWorldProvider->get) == std:("function")}
                    });
                }
            }
            );
            it(std:("should have a description explaining its purpose"), [=]() mutable
            {
                if (AND((helloWorldProvider), (helloWorldProvider->description))) {
                    expect(type_of(helloWorldProvider->description))->toBe(std:("string"));
                    expect(helloWorldProvider->description->length)->toBeGreaterThan(0);
                    documentTestResult(std:("Provider description check"), object{
                        object::pair{std:("description"), helloWorldProvider->description}
                    });
                }
            }
            );
            it(std:("should return provider data from the get method"), [=]() mutable
            {
                if (helloWorldProvider) {
                    auto runtime = createRealRuntime();
                    auto message = createRealMemory();
                    auto state = as<std::shared_ptr<State>>(object{
                        object::pair{std:("values"), object{
                            object::pair{std:("example"), std:("test value")}
                        }}, 
                        object::pair{std:("data"), object{
                            object::pair{std:("additionalContext"), std:("some context")}
                        }}, 
                        object::pair{std:("text"), std:("Current state context")}
                    });
                    auto result = nullptr;
                    auto error = nullptr;
                    try
                    {
                        logger->info(std:("Calling provider.get with real implementation"));
                        result = std::async([=]() { helloWorldProvider->get(runtime, message, state); });
                        expect(result)->toBeDefined();
                        expect(result)->toHaveProperty(std:("text"));
                        expect(result)->toHaveProperty(std:("values"));
                        expect(result)->toHaveProperty(std:("data"));
                        if (AND((result), ((OR((!result["text"]), (result["text"]["length"] == 0)))))) {
                            logger->warn(std:("Provider returned empty text"));
                        }
                        if (AND((result), (Object->keys(result["values"])->get_length() == 0))) {
                            logger->warn(std:("Provider returned empty values object"));
                        }
                        if (AND((result), (Object->keys(result["data"])->get_length() == 0))) {
                            logger->warn(std:("Provider returned empty data object"));
                        }
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std:("Error in provider.get:"), e);
                    }
                    documentTestResult(std:("Provider get method"), result, error);
                }
            }
            );
            it(std:("should handle error conditions gracefully"), [=]() mutable
            {
                if (helloWorldProvider) {
                    auto runtime = createRealRuntime();
                    auto invalidMemory = as<std::shared_ptr<Memory>>(as<any>(object{
                        object::pair{std:("id"), uuidv4()}
                    }));
                    auto state = as<std::shared_ptr<State>>(object{
                        object::pair{std:("values"), object{}}, 
                        object::pair{std:("data"), object{}}, 
                        object::pair{std:("text"), string_empty}
                    });
                    auto result = nullptr;
                    auto error = nullptr;
                    try
                    {
                        logger->info(std:("Calling provider.get with invalid memory object"));
                        result = std::async([=]() { helloWorldProvider->get(runtime, invalidMemory, state); });
                        expect(result)->toBeDefined();
                        logger->info(std:("Provider handled invalid input without throwing"));
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std:("Provider threw an error with invalid input:"), e);
                    }
                    documentTestResult(std:("Provider error handling"), result, error);
                }
            }
            );
        }
        );
        describe(std:("Provider Registration"), [=]() mutable
        {
            it(std:("should include providers in the plugin definition"), [=]() mutable
            {
                expect(plugin)->toHaveProperty(std:("providers"));
                expect(Array->isArray(plugin->providers))->toBe(true);
                documentTestResult(std:("Plugin providers check"), object{
                    object::pair{std:("hasProviders"), !!plugin->providers}, 
                    object::pair{std:("providersCount"), OR((plugin->providers->length), (0))}
                });
            }
            );
            it(std:("should correctly initialize providers array"), [=]() mutable
            {
                if (plugin->providers) {
                    expect(plugin->providers->length)->toBeGreaterThan(0);
                    shared allValid = true;
                    shared invalidProviders = array<string>();
                    plugin->providers->forEach([=](auto provider) mutable
                    {
                        auto isValid = AND((AND((provider->name != undefined), (provider->description != undefined))), (type_of(provider->get) == std:("function")));
                        if (!isValid) {
                            allValid = false;
                            invalidProviders->push(OR((provider->name), (std:("unnamed"))));
                        }
                        expect(provider)->toHaveProperty(std:("name"));
                        expect(provider)->toHaveProperty(std:("description"));
                        expect(provider)->toHaveProperty(std:("get"));
                        expect(type_of(provider->get))->toBe(std:("function"));
                    }
                    );
                    documentTestResult(std:("Provider initialization check"), object{
                        object::pair{std:("providersCount"), plugin->providers->length}, 
                        object::pair{std:("allValid"), std:("allValid")}, 
                        object::pair{std:("invalidProviders"), std:("invalidProviders")}
                    });
                }
            }
            );
            it(std:("should have unique provider names"), [=]() mutable
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
                    documentTestResult(std:("Provider uniqueness check"), object{
                        object::pair{std:("totalProviders"), providerNames->length}, 
                        object::pair{std:("uniqueProviders"), uniqueNames->size}, 
                        object::pair{std:("duplicates"), std:("duplicates")}
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
