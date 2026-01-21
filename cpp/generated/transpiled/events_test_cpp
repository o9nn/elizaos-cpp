#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/events.test.h"

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        auto actual = require(std::string("@elizaos/core"));
        return utils::assign(object{
            , 
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("error"), mock()}
            }}
        }, actual);
    }
    );
    describe(std::string("Plugin Events"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std::string("should have events defined"), [=]() mutable
        {
            expect(plugin->events)->toBeDefined();
            if (plugin->events) {
                expect(Object->keys(plugin->events)->get_length())->toBeGreaterThan(0);
            }
        }
        );
        it(std::string("should handle MESSAGE_RECEIVED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->MESSAGE_RECEIVED))) {
                expect(Array->isArray(plugin->events->MESSAGE_RECEIVED))->toBe(true);
                expect(plugin->events->MESSAGE_RECEIVED->length)->toBeGreaterThan(0);
                auto messageHandler = const_(plugin->events->MESSAGE_RECEIVED)[0];
                expect(type_of(messageHandler))->toBe(std::string("function"));
                auto mockParams = object{
                    object::pair{std::string("message"), object{
                        object::pair{std::string("id"), std::string("test-id")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hello!")}
                        }}
                    }}, 
                    object::pair{std::string("source"), std::string("test")}, 
                    object::pair{std::string("runtime"), object{}}
                };
                std::async([=]() { messageHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std::string("MESSAGE_RECEIVED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
        it(std::string("should handle VOICE_MESSAGE_RECEIVED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->VOICE_MESSAGE_RECEIVED))) {
                expect(Array->isArray(plugin->events->VOICE_MESSAGE_RECEIVED))->toBe(true);
                expect(plugin->events->VOICE_MESSAGE_RECEIVED->length)->toBeGreaterThan(0);
                auto voiceHandler = const_(plugin->events->VOICE_MESSAGE_RECEIVED)[0];
                expect(type_of(voiceHandler))->toBe(std::string("function"));
                auto mockParams = object{
                    object::pair{std::string("message"), object{
                        object::pair{std::string("id"), std::string("test-id")}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Voice message!")}
                        }}
                    }}, 
                    object::pair{std::string("source"), std::string("test")}, 
                    object::pair{std::string("runtime"), object{}}
                };
                std::async([=]() { voiceHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std::string("VOICE_MESSAGE_RECEIVED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
        it(std::string("should handle WORLD_CONNECTED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->WORLD_CONNECTED))) {
                expect(Array->isArray(plugin->events->WORLD_CONNECTED))->toBe(true);
                expect(plugin->events->WORLD_CONNECTED->length)->toBeGreaterThan(0);
                auto connectedHandler = const_(plugin->events->WORLD_CONNECTED)[0];
                expect(type_of(connectedHandler))->toBe(std::string("function"));
                auto mockParams = object{
                    object::pair{std::string("world"), object{
                        object::pair{std::string("id"), std::string("test-world-id")}, 
                        object::pair{std::string("name"), std::string("Test World")}
                    }}, 
                    object::pair{std::string("rooms"), array<any>()}, 
                    object::pair{std::string("entities"), array<any>()}, 
                    object::pair{std::string("source"), std::string("test")}, 
                    object::pair{std::string("runtime"), object{}}
                };
                std::async([=]() { connectedHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std::string("WORLD_CONNECTED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
        it(std::string("should handle WORLD_JOINED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->WORLD_JOINED))) {
                expect(Array->isArray(plugin->events->WORLD_JOINED))->toBe(true);
                expect(plugin->events->WORLD_JOINED->length)->toBeGreaterThan(0);
                auto joinedHandler = const_(plugin->events->WORLD_JOINED)[0];
                expect(type_of(joinedHandler))->toBe(std::string("function"));
                auto mockParams = object{
                    object::pair{std::string("world"), object{
                        object::pair{std::string("id"), std::string("test-world-id")}, 
                        object::pair{std::string("name"), std::string("Test World")}
                    }}, 
                    object::pair{std::string("entity"), object{
                        object::pair{std::string("id"), std::string("test-entity-id")}, 
                        object::pair{std::string("name"), std::string("Test Entity")}
                    }}, 
                    object::pair{std::string("rooms"), array<any>()}, 
                    object::pair{std::string("entities"), array<any>()}, 
                    object::pair{std::string("source"), std::string("test")}, 
                    object::pair{std::string("runtime"), object{}}
                };
                std::async([=]() { joinedHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std::string("WORLD_JOINED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
    }
    );
}

MAIN
