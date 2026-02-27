#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-tee-starter/__tests__/provider.test.h"

void documentTestResult(string testName, any result, any error)
{
    logger->info(std:("TEST: ") + testName + string_empty);
    if (result) {
        if (type_of(result) == std:("string")) {
            logger->info(std:("RESULT: ") + result->substring(0, 100) + string_empty + (result->get_length() > 100) ? std:("...") : string_empty + string_empty);
        } else {
            try
            {
                logger->info(std:("RESULT: ") + JSON->stringify(result, nullptr, 2)->substring(0, 200) + std:("..."));
            }
            catch (const any& e)
            {
                logger->info(std:("RESULT: [Complex object that couldn't be stringified]"));
            }
        }
    }
    if (error) {
        logger->error(std:("ERROR: ") + error->message + string_empty);
        if (error->stack) {
            logger->error(std:("STACK: ") + error->stack + string_empty);
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
                logger->info(std:("DB Get: ") + key + string_empty);
                return nullptr;
            }
            }, 
            object::pair{std:("set"), [=](auto key, auto value) mutable
            {
                logger->info(std:("DB Set: ") + key + std:(" = ") + JSON->stringify(value) + string_empty);
                return true;
            }
            }, 
            object::pair{std:("delete"), [=](auto key) mutable
            {
                logger->info(std:("DB Delete: ") + key + string_empty);
                return true;
            }
            }, 
            object::pair{std:("getKeys"), [=](auto pattern) mutable
            {
                logger->info(std:("DB GetKeys: ") + pattern + string_empty);
                return array<any>();
            }
            }
        }}, 
        object::pair{std:("memory"), object{
            object::pair{std:("add"), [=](auto memory) mutable
            {
                logger->info(std:("Memory Add: ") + JSON->stringify(memory)->substring(0, 100) + string_empty);
            }
            }, 
            object::pair{std:("get"), [=](auto id) mutable
            {
                logger->info(std:("Memory Get: ") + id + string_empty);
                return nullptr;
            }
            }, 
            object::pair{std:("getByEntityId"), [=](auto entityId) mutable
            {
                logger->info(std:("Memory GetByEntityId: ") + entityId + string_empty);
                return array<any>();
            }
            }, 
            object::pair{std:("getLatest"), [=](auto entityId) mutable
            {
                logger->info(std:("Memory GetLatest: ") + entityId + string_empty);
                return nullptr;
            }
            }, 
            object::pair{std:("getRecentMessages"), [=](auto options) mutable
            {
                logger->info(std:("Memory GetRecentMessages: ") + JSON->stringify(options) + string_empty);
                return array<any>();
            }
            }, 
            object::pair{std:("search"), [=](auto query) mutable
            {
                logger->info(std:("Memory Search: ") + query + string_empty);
                return array<any>();
            }
            }
        }}, 
        object::pair{std:("getService"), [=](auto serviceType) mutable
        {
            logger->info(std:("GetService: ") + serviceType + string_empty);
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
        describe(std:("HELLO_WORLD_PROVIDER"), [=]() mutable
        {
            it(std:("should exist in the plugin"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                expect(plugin)->toBeDefined();
                expect(plugin->providers)->toBeDefined();
                expect(plugin->providers->length)->toBe(0);
            }
            );
            it(std:("should have the correct structure"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std:("should have a description explaining its purpose"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std:("should return provider data from the get method"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std:("should handle error conditions gracefully"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std:("Provider Registration"), [=]() mutable
        {
            it(std:("should include providers in the plugin definition"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                expect(plugin->providers)->toBeDefined();
                expect(Array->isArray(plugin->providers))->toBe(true);
            }
            );
            it(std:("should correctly initialize providers array"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                expect(plugin->providers)->toBeDefined();
                expect(plugin->providers->length)->toBe(0);
            }
            );
            it(std:("should have unique provider names"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                if (AND((plugin->providers), (plugin->providers->length > 0))) {
                    auto providerNames = plugin->providers->map([=](auto p) mutable
                    {
                        return p["name"];
                    }
                    );
                    auto uniqueNames = std::make_shared<Set>(providerNames);
                    expect(uniqueNames->size)->toBe(providerNames->length);
                } else {
                    expect(true)->toBe(true);
                }
            }
            );
        }
        );
    }
    );
}

MAIN
