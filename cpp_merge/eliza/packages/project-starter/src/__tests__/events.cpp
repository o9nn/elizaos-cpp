#include "events.test.h"

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        auto actual = require(std:("@elizaos/core"));
        return utils::assign(object{
            , 
            object::pair{std:("logger"), object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("error"), mock()}
            }}
        }, actual);
    }
    );
    describe(std:("Plugin Events"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            mock->restore();
        }
        );
        it(std:("should have events defined"), [=]() mutable
        {
            expect(plugin->events)->toBeDefined();
            if (plugin->events) {
                expect(Object->keys(plugin->events)->get_length())->toBeGreaterThan(0);
            }
        }
        );
        it(std:("should handle MESSAGE_RECEIVED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->MESSAGE_RECEIVED))) {
                expect(Array->isArray(plugin->events->MESSAGE_RECEIVED))->toBe(true);
                expect(plugin->events->MESSAGE_RECEIVED->length)->toBeGreaterThan(0);
                auto messageHandler = const_(plugin->events->MESSAGE_RECEIVED)[0];
                expect(type_of(messageHandler))->toBe(std:("function"));
                auto mockParams = object{
                    object::pair{std:("message"), object{
                        object::pair{std:("id"), std:("test-id")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hello!")}
                        }}
                    }}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("runtime"), object{}}
                };
                std::async([=]() { messageHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std:("MESSAGE_RECEIVED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
        it(std:("should handle VOICE_MESSAGE_RECEIVED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->VOICE_MESSAGE_RECEIVED))) {
                expect(Array->isArray(plugin->events->VOICE_MESSAGE_RECEIVED))->toBe(true);
                expect(plugin->events->VOICE_MESSAGE_RECEIVED->length)->toBeGreaterThan(0);
                auto voiceHandler = const_(plugin->events->VOICE_MESSAGE_RECEIVED)[0];
                expect(type_of(voiceHandler))->toBe(std:("function"));
                auto mockParams = object{
                    object::pair{std:("message"), object{
                        object::pair{std:("id"), std:("test-id")}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Voice message!")}
                        }}
                    }}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("runtime"), object{}}
                };
                std::async([=]() { voiceHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std:("VOICE_MESSAGE_RECEIVED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
        it(std:("should handle WORLD_CONNECTED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->WORLD_CONNECTED))) {
                expect(Array->isArray(plugin->events->WORLD_CONNECTED))->toBe(true);
                expect(plugin->events->WORLD_CONNECTED->length)->toBeGreaterThan(0);
                auto connectedHandler = const_(plugin->events->WORLD_CONNECTED)[0];
                expect(type_of(connectedHandler))->toBe(std:("function"));
                auto mockParams = object{
                    object::pair{std:("world"), object{
                        object::pair{std:("id"), std:("test-world-id")}, 
                        object::pair{std:("name"), std:("Test World")}
                    }}, 
                    object::pair{std:("rooms"), array<any>()}, 
                    object::pair{std:("entities"), array<any>()}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("runtime"), object{}}
                };
                std::async([=]() { connectedHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std:("WORLD_CONNECTED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
        it(std:("should handle WORLD_JOINED event"), [=]() mutable
        {
            if (AND((plugin->events), (plugin->events->WORLD_JOINED))) {
                expect(Array->isArray(plugin->events->WORLD_JOINED))->toBe(true);
                expect(plugin->events->WORLD_JOINED->length)->toBeGreaterThan(0);
                auto joinedHandler = const_(plugin->events->WORLD_JOINED)[0];
                expect(type_of(joinedHandler))->toBe(std:("function"));
                auto mockParams = object{
                    object::pair{std:("world"), object{
                        object::pair{std:("id"), std:("test-world-id")}, 
                        object::pair{std:("name"), std:("Test World")}
                    }}, 
                    object::pair{std:("entity"), object{
                        object::pair{std:("id"), std:("test-entity-id")}, 
                        object::pair{std:("name"), std:("Test Entity")}
                    }}, 
                    object::pair{std:("rooms"), array<any>()}, 
                    object::pair{std:("entities"), array<any>()}, 
                    object::pair{std:("source"), std:("test")}, 
                    object::pair{std:("runtime"), object{}}
                };
                std::async([=]() { joinedHandler(mockParams); });
                expect(logger->info)->toHaveBeenCalledWith(std:("WORLD_JOINED event received"));
                expect(logger->info)->toHaveBeenCalledWith(expect->any(Array));
            }
        }
        );
    }
    );
}

MAIN
