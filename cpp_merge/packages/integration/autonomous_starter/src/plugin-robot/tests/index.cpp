#include "index.test.h"

void Main(void)
{
    vi->mock(std:("@jitsi/robotjs"), [=]() mutable
    {
        return (object{
            object::pair{std:("default"), object{
                object::pair{std:("getScreenSize"), vi->fn([=]() mutable
                {
                    return (object{
                        object::pair{std:("width"), 1920}, 
                        object::pair{std:("height"), 1080}
                    });
                }
                )}, 
                object::pair{std:("screen"), object{
                    object::pair{std:("capture"), vi->fn([=]() mutable
                    {
                        return (object{
                            object::pair{std:("image"), Buffer::from(std:("mock-screenshot-data"))}, 
                            object::pair{std:("width"), 1920}, 
                            object::pair{std:("height"), 1080}, 
                            object::pair{std:("byteWidth"), 7680}, 
                            object::pair{std:("bitsPerPixel"), 32}, 
                            object::pair{std:("bytesPerPixel"), 4}
                        });
                    }
                    )}
                }}, 
                object::pair{std:("moveMouse"), vi->fn()}, 
                object::pair{std:("mouseClick"), vi->fn()}, 
                object::pair{std:("typeString"), vi->fn()}
            }}
        });
    }
    );
    describe(std:("Robot Plugin"), [=]() mutable
    {
        describe(std:("plugin structure"), [=]() mutable
        {
            it(std:("should have correct plugin properties"), [=]() mutable
            {
                expect(robotPlugin->name)->toBe(std:("plugin-robot"));
                expect(robotPlugin->description)->toBe(std:("Control screen using robotjs and provide screen context"));
            }
            );
            it(std:("should correct components"), [=]() mutable
            {
                expect(robotPlugin->actions)->toHaveLength(1);
                expect(robotPlugin->providers)->toHaveLength(1);
                expect(robotPlugin->services)->toHaveLength(1);
                expect(const_(robotPlugin->actions)[0])->toBe(performScreenAction);
                expect(const_(robotPlugin->providers)[0])->toBe(screenProvider);
                expect(const_(robotPlugin->services)[0])->toBe(RobotService);
            }
            );
            it(std:("should have valid action structure"), [=]() mutable
            {
                expect(robotPlugin->actions)->toBeInstanceOf(Array);
                expect(robotPlugin->actions->length)->toBeGreaterThan(0);
                auto action = const_(robotPlugin->actions)[0];
                expect(action->name)->toBe(std:("PERFORM_SCREEN_ACTION"));
                expect(action->similes)->toEqual(array<string>{ std:("SCREEN_ACTION"), std:("CONTROL_SCREEN"), std:("INTERACT_SCREEN") });
                expect(action->description)->toContain(std:("Perform mouse and keyboard actions"));
                expect(type_of(action->validate))->toBe(std:("function"));
                expect(type_of(action->handler))->toBe(std:("function"));
                expect(action->examples)->toBeDefined();
            }
            );
            it(std:("should have valid provider structure"), [=]() mutable
            {
                expect(robotPlugin->providers)->toBeInstanceOf(Array);
                expect(robotPlugin->providers->length)->toBeGreaterThan(0);
                auto provider = const_(robotPlugin->providers)[0];
                expect(provider->name)->toBe(std:("SCREEN_CONTEXT"));
                expect(provider->description)->toBe(std:("Current screen context with OCR, description history, and change detection information."));
                expect(provider->position)->toBe(50);
                expect(type_of(provider->get))->toBe(std:("function"));
            }
            );
            it(std:("should have valid service structure"), [=]() mutable
            {
                auto service = const_(robotPlugin->services)[0];
                expect(service->serviceType)->toBe(std:("ROBOT"));
                expect(type_of(service->start))->toBe(std:("function"));
            }
            );
        }
        );
    }
    );
}

MAIN
