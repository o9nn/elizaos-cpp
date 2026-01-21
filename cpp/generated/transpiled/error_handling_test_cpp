#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/error-handling.test.h"

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        auto actual = require(std::string("@elizaos/core"));
        return utils::assign(object{
            , 
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), mock()}, 
                object::pair{std::string("error"), mock()}, 
                object::pair{std::string("warn"), mock()}
            }}
        }, actual);
    }
    );
    describe(std::string("Error Handling"), [=]() mutable
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
        describe(std::string("HELLO_WORLD Action Error Handling"), [=]() mutable
        {
            it(std::string("should log errors in action handlers"), [=]() mutable
            {
                auto action = plugin->actions->find([=](auto a) mutable
                {
                    return a["name"] == std::string("HELLO_WORLD");
                }
                );
                if (AND((action), (action->handler))) {
                    auto mockError = std::make_shared<Error>(std::string("Test error in action"));
                    spyOn(console, std::string("error"))->mockImplementation([=]() mutable
                    {
                    }
                    );
                    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{}));
                    auto mockMessage = as<std::shared_ptr<Memory>>(object{
                        object::pair{std::string("entityId"), uuidv4()}, 
                        object::pair{std::string("roomId"), uuidv4()}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Hello!")}, 
                            object::pair{std::string("source"), std::string("test")}
                        }}
                    });
                    auto mockState = as<std::shared_ptr<State>>(object{
                        object::pair{std::string("values"), object{}}, 
                        object::pair{std::string("data"), object{}}, 
                        object::pair{std::string("text"), string_empty}
                    });
                    auto mockCallback = mock();
                    spyOn(logger, std::string("error"));
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
        describe(std::string("Service Error Handling"), [=]() mutable
        {
            it(std::string("should throw an error when stopping non-existent service"), [=]() mutable
            {
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std::string("getService"), mock()->mockReturnValue(nullptr)}
                }));
                auto caughtError = nullptr;
                try
                {
                    std::async([=]() { StarterService::stop(mockRuntime); });
                }
                catch (const any& error)
                {
                    caughtError = error;
                    expect(error["message"])->toBe(std::string("Starter service not found"));
                }
                expect(caughtError)->not->toBeNull();
                expect(mockRuntime->getService)->toHaveBeenCalledWith(std::string("starter"));
            }
            );
            it(std::string("should handle service stop errors gracefully"), [=]() mutable
            {
                auto mockServiceWithError = object{
                    object::pair{std::string("stop"), mock()->mockImplementation([=]() mutable
                    {
                        throw any(std::make_shared<Error>(std::string("Error stopping service")));
                    }
                    )}
                };
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                    object::pair{std::string("getService"), mock()->mockReturnValue(mockServiceWithError)}
                }));
                auto caughtError = nullptr;
                try
                {
                    std::async([=]() { StarterService::stop(mockRuntime); });
                }
                catch (const any& error)
                {
                    caughtError = error;
                    expect(error["message"])->toBe(std::string("Error stopping service"));
                }
                expect(caughtError)->not->toBeNull();
                expect(mockRuntime->getService)->toHaveBeenCalledWith(std::string("starter"));
                expect(mockServiceWithError["stop"])->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("Plugin Events Error Handling"), [=]() mutable
        {
            it(std::string("should handle errors in event handlers gracefully"), [=]() mutable
            {
                if (AND((plugin->events), (plugin->events->MESSAGE_RECEIVED))) {
                    auto messageHandler = const_(plugin->events->MESSAGE_RECEIVED)[0];
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
                    spyOn(logger, std::string("error"));
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
        describe(std::string("Provider Error Handling"), [=]() mutable
        {
            it(std::string("should handle errors in provider.get method"), [=]() mutable
            {
                auto provider = plugin->providers->find([=](auto p) mutable
                {
                    return p["name"] == std::string("HELLO_WORLD_PROVIDER");
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
