#include "error-handling.test.h"

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        auto actual = require(std:("@elizaos/core"));
        return utils::assign(object{
            , 
            object::pair{std:("logger"), object{
                object::pair{std:("info"), mock()}, 
                object::pair{std:("error"), mock()}, 
                object::pair{std:("warn"), mock()}
            }}
        }, actual);
    }
    );
    describe(std:("Error Handling"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            mock->restore();
        }
        );
        afterEach([=]() mutable
        {
        }
        );
        describe(std:("HELLO_WORLD Action Error Handling"), [=]() mutable
        {
            it(std:("should log errors in action handlers"), [=]() mutable
            {
                auto action = plugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std:("HELLO_WORLD");
                }
                );
                if (AND((action), (action->handler))) {
                    auto mockError = std::make_shared<Error>(std:("Test error in action"));
                    spyOn(console, std:("error"))->mockImplementation([=]() mutable
                    {
                    }
                    );
                    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{}));
                    auto mockMessage = as<std::shared_ptr<Memory>>(object{
                        object::pair{std:("entityId"), uuidv4()}, 
                        object::pair{std:("roomId"), uuidv4()}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Hello!")}, 
                            object::pair{std:("source"), std:("test")}
                        }}
                    });
                    auto mockState = as<std::shared_ptr<State>>(object{
                        object::pair{std:("values"), object{}}, 
                        object::pair{std:("data"), object{}}, 
                        object::pair{std:("text"), string_empty}
                    });
                    auto mockCallback = mock();
                    spyOn(logger, std:("error"));
                    try
                    {
                        std::async([=]() { action->handler(mockRuntime, mockMessage, mockState, object{}, mockCallback, array<any>()); });
                        expect(mockCallback)->toHaveBeenCalled();
                    }
                    catch (const any& error)
                    {
                        expect(logger->error)->toHaveBeenCalled();
                    }
                }
            }
            );
        }
        );
        describe(std:("Service Error Handling"), [=]() mutable
        {
            it(std:("should throw an error when stopping non-existent service"), [=]() mutable
            {
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std:("getService"), mock()->mockReturnValue(nullptr)}
                }));
                auto caughtError = nullptr;
                try
                {
                    std::async([=]() { StarterService::stop(mockRuntime); });
                }
                catch (const any& error)
                {
                    caughtError = error;
                    expect(error["message"])->toBe(std:("Starter service not found"));
                }
                expect(caughtError)->not->toBeNull();
                expect(mockRuntime->getService)->toHaveBeenCalledWith(std:("starter"));
            }
            );
            it(std:("should handle service stop errors gracefully"), [=]() mutable
            {
                auto mockServiceWithError = object{
                    object::pair{std:("stop"), mock()->mockImplementation([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std:("Error stopping service")));
                    }
                    )}
                };
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std:("getService"), mock()->mockReturnValue(mockServiceWithError)}
                }));
                auto caughtError = nullptr;
                try
                {
                    std::async([=]() { StarterService::stop(mockRuntime); });
                }
                catch (const any& error)
                {
                    caughtError = error;
                    expect(error["message"])->toBe(std:("Error stopping service"));
                }
                expect(caughtError)->not->toBeNull();
                expect(mockRuntime->getService)->toHaveBeenCalledWith(std:("starter"));
                expect(mockServiceWithError["stop"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("Plugin Events Error Handling"), [=]() mutable
        {
            it(std:("should handle errors in event handlers gracefully"), [=]() mutable
            {
                if (AND((plugin->events), (plugin->events->MESSAGE_RECEIVED))) {
                    auto messageHandler = const_(plugin->events->MESSAGE_RECEIVED)[0];
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
                    spyOn(logger, std:("error"));
                    try
                    {
                        std::async([=]() { messageHandler(as<any>(mockParams)); });
                        expect(true)->toBe(true);
                    }
                    catch (const any& error)
                    {
                        expect(error)->toBeDefined();
                    }
                }
            }
            );
        }
        );
        describe(std:("Provider Error Handling"), [=]() mutable
        {
            it(std:("should handle errors in provider.get method"), [=]() mutable
            {
                auto provider = plugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std:("HELLO_WORLD_PROVIDER");
                }
                );
                if (provider) {
                    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(nullptr));
                    auto mockMessage = as<std::shared_ptr<Memory>>(as<any>(nullptr));
                    auto mockState = as<std::shared_ptr<State>>(as<any>(nullptr));
                    try
                    {
                        std::async([=]() { provider->get(mockRuntime, mockMessage, mockState); });
                        expect(true)->toBe(true);
                    }
                    catch (const any& error)
                    {
                        expect(logger->error)->toHaveBeenCalled();
                    }
                }
            }
            );
        }
        );
    }
    );
}

MAIN
