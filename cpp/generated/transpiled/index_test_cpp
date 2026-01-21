#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/index.test.h"

void Main(void)
{
    vi->mock(std::string("@jitsi/robotjs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("default"), object{
                object::pair{std::string("getScreenSize"), vi->fn([=]() mutable
                {
                    return (object{
                        object::pair{std::string("width"), 1920}, 
                        object::pair{std::string("height"), 1080}
                    });
                }
                )}, 
                object::pair{std::string("screen"), object{
                    object::pair{std::string("capture"), vi->fn([=]() mutable
                    {
                        return (object{
                            object::pair{std::string("image"), Buffer::from(std::string("mock-screenshot-data"))}, 
                            object::pair{std::string("width"), 1920}, 
                            object::pair{std::string("height"), 1080}, 
                            object::pair{std::string("byteWidth"), 7680}, 
                            object::pair{std::string("bitsPerPixel"), 32}, 
                            object::pair{std::string("bytesPerPixel"), 4}
                        });
                    }
                    )}
                }}, 
                object::pair{std::string("moveMouse"), vi->fn()}, 
                object::pair{std::string("mouseClick"), vi->fn()}, 
                object::pair{std::string("typeString"), vi->fn()}
            }}
        });
    }
    );
    describe(std::string("Robot Plugin"), [=]() mutable
    {
        describe(std::string("plugin structure"), [=]() mutable
        {
            it(std::string("should have correct plugin properties"), [=]() mutable
            {
                expect(robotPlugin->name)->toBe(std::string("plugin-robot"));
                expect(robotPlugin->description)->toBe(std::string("Control screen using robotjs and provide screen context"));
            }
            );
            it(std::string("should export correct components"), [=]() mutable
            {
                expect(robotPlugin->actions)->toHaveLength(1);
                expect(robotPlugin->providers)->toHaveLength(1);
                expect(robotPlugin->services)->toHaveLength(1);
                expect(const_(robotPlugin->actions)[0])->toBe(performScreenAction);
                expect(const_(robotPlugin->providers)[0])->toBe(screenProvider);
                expect(const_(robotPlugin->services)[0])->toBe(RobotService);
            }
            );
            it(std::string("should have valid action structure"), [=]() mutable
            {
                expect(robotPlugin->actions)->toBeInstanceOf(Array);
                expect(robotPlugin->actions->length)->toBeGreaterThan(0);
                auto action = const_(robotPlugin->actions)[0];
                expect(action->name)->toBe(std::string("PERFORM_SCREEN_ACTION"));
                expect(action->similes)->toEqual(array<string>{ std::string("SCREEN_ACTION"), std::string("CONTROL_SCREEN"), std::string("INTERACT_SCREEN") });
                expect(action->description)->toContain(std::string("Perform mouse and keyboard actions"));
                expect(type_of(action->validate))->toBe(std::string("function"));
                expect(type_of(action->handler))->toBe(std::string("function"));
                expect(action->examples)->toBeDefined();
            }
            );
            it(std::string("should have valid provider structure"), [=]() mutable
            {
                expect(robotPlugin->providers)->toBeInstanceOf(Array);
                expect(robotPlugin->providers->length)->toBeGreaterThan(0);
                auto provider = const_(robotPlugin->providers)[0];
                expect(provider->name)->toBe(std::string("SCREEN_CONTEXT"));
                expect(provider->description)->toBe(std::string("Current screen context with OCR, description history, and change detection information."));
                expect(provider->position)->toBe(50);
                expect(type_of(provider->get))->toBe(std::string("function"));
            }
            );
            it(std::string("should have valid service structure"), [=]() mutable
            {
                auto service = const_(robotPlugin->services)[0];
                expect(service->serviceType)->toBe(std::string("ROBOT"));
                expect(type_of(service->start))->toBe(std::string("function"));
            }
            );
        }
        );
    }
    );
}

MAIN
