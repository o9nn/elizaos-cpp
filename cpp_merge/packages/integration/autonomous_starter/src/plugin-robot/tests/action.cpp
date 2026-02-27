#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/action.test.h"

std::shared_ptr<RobotService> mockRobotService = as<std::shared_ptr<RobotService>>(as<any>(object{
    object::pair{std:("moveMouse"), vi->fn()}, 
    object::pair{std:("click"), vi->fn()}, 
    object::pair{std:("typeText"), vi->fn()}, 
    object::pair{std:("getContext"), vi->fn()}, 
    object::pair{std:("updateContext"), vi->fn()}, 
    object::pair{std:("stop"), vi->fn()}, 
    object::pair{std:("capabilityDescription"), std:("Controls the screen and provides recent screen context.")}
}));
any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std:("agentId"), as<std::shared_ptr<const>>(std:("12345678-1234-1234-1234-123456789abc"))}, 
    object::pair{std:("getService"), vi->fn([=]() mutable
    {
        return mockRobotService;
    }
    )}, 
    object::pair{std:("useModel"), vi->fn()}, 
    object::pair{std:("emitEvent"), vi->fn()}
}));
std::function<any(string)> createMockMessage = [=](auto text) mutable
{
    return (object{
        object::pair{std:("id"), std:("12345678-1234-1234-1234-123456789abc")}, 
        object::pair{std:("agentId"), std:("agent-12345678-1234-1234-1234-123456789abc")}, 
        object::pair{std:("entityId"), std:("entity-12345678-1234-1234-1234-123456789def")}, 
        object::pair{std:("roomId"), std:("room-12345678-1234-1234-1234-123456789ghi")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("text")}
        }}, 
        object::pair{std:("createdAt"), Date->now()}
    });
};
std::function<any(std::shared_ptr<Record<string, any>>)> createMockState = [=](auto additionalData = object{}) mutable
{
    return (utils::assign(object{
        object::pair{std:("values"), object{}}, 
        object::pair{std:("data"), object{}}, 
        object::pair{std:("text"), string_empty}
    }, additionalData));
};

void Main(void)
{
    describe(std:("performScreenAction"), [=]() mutable
    {
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockCallback = vi->fn();
        }
        );
        describe(std:("action properties"), [=]() mutable
        {
            it(std:("should have correct action properties"), [=]() mutable
            {
                expect(performScreenAction->name)->toBe(std:("PERFORM_SCREEN_ACTION"));
                expect(performScreenAction->similes)->toEqual(array<string>{ std:("SCREEN_ACTION"), std:("CONTROL_SCREEN"), std:("INTERACT_SCREEN") });
                expect(performScreenAction->description)->toContain(std:("Perform mouse and keyboard actions"));
            }
            );
            it(std:("should have examples"), [=]() mutable
            {
                expect(performScreenAction->examples)->toBeDefined();
                expect(performScreenAction->examples)->toHaveLength(2);
                expect(const_(performScreenAction->examples)[0])->toHaveLength(2);
                expect(const_(performScreenAction->examples)[1])->toHaveLength(2);
            }
            );
        }
        );
        describe(std:("validate"), [=]() mutable
        {
            it(std:("should validate successfully when RobotService is available"), [=]() mutable
            {
                auto message = createMockMessage(std:("test"));
                auto isValid = std::async([=]() { performScreenAction->validate(mockRuntime, message); });
                expect(isValid)->toBe(true);
                expect(mockRuntime->getService)->toHaveBeenCalledWith(std:("ROBOT"));
            }
            );
            it(std:("should fail validation when RobotService is not available"), [=]() mutable
            {
                auto runtimeWithoutService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn([=]() mutable
                    {
                        return nullptr;
                    }
                    )}
                }, mockRuntime)));
                auto message = createMockMessage(std:("test"));
                auto isValid = std::async([=]() { performScreenAction->validate(runtimeWithoutService, message); });
                expect(isValid)->toBe(false);
            }
            );
        }
        );
        describe(std:("handler"), [=]() mutable
        {
            shared message = createMockMessage(std:("click submit button"));
            shared state = createMockState();
            it(std:("should handle mouse move action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("x"), 100}, 
                        object::pair{std:("y"), 200}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(100, 200);
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("thought"), std:("Executed 1 screen actions successfully")}, 
                    object::pair{std:("text"), std:("Screen actions completed: moved mouse to (100, 200).")}
                });
            }
            );
            it(std:("should handle click action with default button"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("left"), false);
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("thought"), std:("Executed 1 screen actions successfully")}, 
                    object::pair{std:("text"), std:("Screen actions completed: clicked left mouse button.")}
                });
            }
            );
            it(std:("should handle click action with specified button"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}, 
                        object::pair{std:("button"), std:("right")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("right"), false);
            }
            );
            it(std:("should handle type action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}, 
                        object::pair{std:("text"), std:("Hello, World!")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std:("Hello, World!"));
            }
            );
            it(std:("should handle multiple actions in sequence"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("x"), 100}, 
                        object::pair{std:("y"), 200}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}, 
                        object::pair{std:("button"), std:("left")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}, 
                        object::pair{std:("text"), std:("test input")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(100, 200);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("left"), false);
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std:("test input"));
            }
            );
            it(std:("should skip invalid move actions (missing coordinates)"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("x"), 100}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("y"), 200}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("x"), 50}, 
                        object::pair{std:("y"), 75}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledTimes(1);
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(50, 75);
            }
            );
            it(std:("should skip type actions without text"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}, 
                        object::pair{std:("text"), string_empty}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}, 
                        object::pair{std:("text"), std:("valid text")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledTimes(1);
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std:("valid text"));
            }
            );
            it(std:("should handle empty steps array"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<any>()}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->not->toHaveBeenCalled();
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("thought"), std:("No valid steps provided")}, 
                    object::pair{std:("text"), std:("Unable to perform screen action - no valid steps were provided.")}
                });
            }
            );
            it(std:("should handle missing steps property"), [=]() mutable
            {
                auto options = object{};
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->not->toHaveBeenCalled();
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("thought"), std:("No valid steps provided")}, 
                    object::pair{std:("text"), std:("Unable to perform screen action - no valid steps were provided.")}
                });
            }
            );
            it(std:("should handle service unavailable gracefully"), [=]() mutable
            {
                auto runtimeWithoutService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn([=]() mutable
                    {
                        return nullptr;
                    }
                    )}
                }, mockRuntime)));
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(runtimeWithoutService, message, state, options, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("thought"), std:("RobotService not available")}, 
                    object::pair{std:("text"), std:("Unable to perform screen action - robot service is not available.")}
                });
            }
            );
            it(std:("should handle unknown action types gracefully"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<any>{ as<any>(object{
                        object::pair{std:("action"), std:("unknown_action")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("left"), false);
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std:("thought"), std:("Executed 1 screen actions successfully")}, 
                    object::pair{std:("text"), std:("Screen actions completed: skipped invalid step: {"action":"unknown_action"}, clicked left mouse button.")}
                });
            }
            );
            it(std:("should handle special characters in type action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}, 
                        object::pair{std:("text"), std:("Special chars: @#$%^&*()_+-=[]{}|;:,.<>?")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std:("Special chars: @#$%^&*()_+-=[]{}|;:,.<>?"));
            }
            );
            it(std:("should handle unicode characters in type action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("type")}, 
                        object::pair{std:("text"), std:("Unicode: 你好 🌟 café naïve")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std:("Unicode: 你好 🌟 café naïve"));
            }
            );
            it(std:("should handle negative coordinates in move action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("x"), -10}, 
                        object::pair{std:("y"), -20}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(-10, -20);
            }
            );
            it(std:("should handle large coordinates in move action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("move")}, 
                        object::pair{std:("x"), 9999}, 
                        object::pair{std:("y"), 9999}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(9999, 9999);
            }
            );
            it(std:("should handle all mouse button types"), [=]() mutable
            {
                auto options = object{
                    object::pair{std:("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}, 
                        object::pair{std:("button"), std:("left")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}, 
                        object::pair{std:("button"), std:("right")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std:("action"), std:("click")}, 
                        object::pair{std:("button"), std:("middle")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("left"), false);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("right"), false);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std:("middle"), false);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledTimes(3);
            }
            );
        }
        );
    }
    );
}

MAIN
