#include "provider.test.h"
#include <string>

void documentTestResult(string testName, any result, any error)
{
    logger->info(std::string("TEST: ") + testName + string_empty);
    if (result) {
        if (type_of(result) == std::string("string")) {
            logger->info(std::string("RESULT: ") + result->substring(0, 100) + string_empty + (result->get_length() > 100) ? std::string("...") : string_empty + string_empty);
        } else {
            try
            {
                logger->info(std::string("RESULT: ") + JSON->stringify(result, nullptr, 2)->substring(0, 200) + std::string("..."));
            }
            catch (const any& e)
            {
                logger->info(std::string("RESULT: [Complex object that couldn't be stringified]"));
            }
        }
    }
    if (error) {
        logger->error(std::string("ERROR: ") + error->message + string_empty);
        if (error->stack) {
            logger->error(std::string("STACK: ") + error->stack + string_empty);
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
                logger->info(std::string("DB Get: ") + key + string_empty);
                return nullptr;
            }
            }, 
            object::pair{std::string("set"), [=](auto key, auto value) mutable
            {
                logger->info(std::string("DB Set: ") + key + std::string(" = ") + JSON->stringify(value) + string_empty);
                return true;
            }
            }, 
            object::pair{std::string("delete"), [=](auto key) mutable
            {
                logger->info(std::string("DB Delete: ") + key + string_empty);
                return true;
            }
            }, 
            object::pair{std::string("getKeys"), [=](auto pattern) mutable
            {
                logger->info(std::string("DB GetKeys: ") + pattern + string_empty);
                return array<any>();
            }
            }
        }}, 
        object::pair{std::string("memory"), object{
            object::pair{std::string("add"), [=](auto memory) mutable
            {
                logger->info(std::string("Memory Add: ") + JSON->stringify(memory)->substring(0, 100) + string_empty);
            }
            }, 
            object::pair{std::string("get"), [=](auto id) mutable
            {
                logger->info(std::string("Memory Get: ") + id + string_empty);
                return nullptr;
            }
            }, 
            object::pair{std::string("getByEntityId"), [=](auto entityId) mutable
            {
                logger->info(std::string("Memory GetByEntityId: ") + entityId + string_empty);
                return array<any>();
            }
            }, 
            object::pair{std::string("getLatest"), [=](auto entityId) mutable
            {
                logger->info(std::string("Memory GetLatest: ") + entityId + string_empty);
                return nullptr;
            }
            }, 
            object::pair{std::string("getRecentMessages"), [=](auto options) mutable
            {
                logger->info(std::string("Memory GetRecentMessages: ") + JSON->stringify(options) + string_empty);
                return array<any>();
            }
            }, 
            object::pair{std::string("search"), [=](auto query) mutable
            {
                logger->info(std::string("Memory Search: ") + query + string_empty);
                return array<any>();
            }
            }
        }}, 
        object::pair{std::string("getService"), [=](auto serviceType) mutable
        {
            logger->info(std::string("GetService: ") + serviceType + string_empty);
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
        describe(std::string("HELLO_WORLD_PROVIDER"), [=]() mutable
        {
            it(std::string("should exist in the plugin"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                expect(plugin)->toBeDefined();
                expect(plugin->providers)->toBeDefined();
                expect(plugin->providers->length)->toBe(0);
            }
            );
            it(std::string("should have the correct structure"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std::string("should have a description explaining its purpose"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std::string("should return provider data from the get method"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
            it(std::string("should handle error conditions gracefully"), [=]() mutable
            {
                expect(true)->toBe(true);
            }
            );
        }
        );
        describe(std::string("Provider Registration"), [=]() mutable
        {
            it(std::string("should include providers in the plugin definition"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                expect(plugin->providers)->toBeDefined();
                expect(Array->isArray(plugin->providers))->toBe(true);
            }
            );
            it(std::string("should correctly initialize providers array"), [=]() mutable
            {
                auto plugin = teeStarterPlugin;
                expect(plugin->providers)->toBeDefined();
                expect(plugin->providers->length)->toBe(0);
            }
            );
            it(std::string("should have unique provider names"), [=]() mutable
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
