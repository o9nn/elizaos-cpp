#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/action.test.h"

std::shared_ptr<RobotService> mockRobotService = as<std::shared_ptr<RobotService>>(as<any>(object{
    object::pair{std::string("moveMouse"), vi->fn()}, 
    object::pair{std::string("click"), vi->fn()}, 
    object::pair{std::string("typeText"), vi->fn()}, 
    object::pair{std::string("getContext"), vi->fn()}, 
    object::pair{std::string("updateContext"), vi->fn()}, 
    object::pair{std::string("stop"), vi->fn()}, 
    object::pair{std::string("capabilityDescription"), std::string("Controls the screen and provides recent screen context.")}
}));
any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std::string("agentId"), as<std::shared_ptr<const>>(std::string("12345678-1234-1234-1234-123456789abc"))}, 
    object::pair{std::string("getService"), vi->fn([=]() mutable
    {
        return mockRobotService;
    }
    )}, 
    object::pair{std::string("useModel"), vi->fn()}, 
    object::pair{std::string("emitEvent"), vi->fn()}
}));
std::function<any(string)> createMockMessage = [=](auto text) mutable
{
    return (object{
        object::pair{std::string("id"), std::string("12345678-1234-1234-1234-123456789abc")}, 
        object::pair{std::string("agentId"), std::string("agent-12345678-1234-1234-1234-123456789abc")}, 
        object::pair{std::string("entityId"), std::string("entity-12345678-1234-1234-1234-123456789def")}, 
        object::pair{std::string("roomId"), std::string("room-12345678-1234-1234-1234-123456789ghi")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("text")}
        }}, 
        object::pair{std::string("createdAt"), Date->now()}
    });
};
std::function<any(std::shared_ptr<Record<string, any>>)> createMockState = [=](auto additionalData = object{}) mutable
{
    return (utils::assign(object{
        object::pair{std::string("values"), object{}}, 
        object::pair{std::string("data"), object{}}, 
        object::pair{std::string("text"), string_empty}
    }, additionalData));
};

void Main(void)
{
    describe(std::string("performScreenAction"), [=]() mutable
    {
        shared<std::shared_ptr<HandlerCallback>> mockCallback;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockCallback = vi->fn();
        }
        );
        describe(std::string("action properties"), [=]() mutable
        {
            it(std::string("should have correct action properties"), [=]() mutable
            {
                expect(performScreenAction->name)->toBe(std::string("PERFORM_SCREEN_ACTION"));
                expect(performScreenAction->similes)->toEqual(array<string>{ std::string("SCREEN_ACTION"), std::string("CONTROL_SCREEN"), std::string("INTERACT_SCREEN") });
                expect(performScreenAction->description)->toContain(std::string("Perform mouse and keyboard actions"));
            }
            );
            it(std::string("should have examples"), [=]() mutable
            {
                expect(performScreenAction->examples)->toBeDefined();
                expect(performScreenAction->examples)->toHaveLength(2);
                expect(const_(performScreenAction->examples)[0])->toHaveLength(2);
                expect(const_(performScreenAction->examples)[1])->toHaveLength(2);
            }
            );
        }
        );
        describe(std::string("validate"), [=]() mutable
        {
            it(std::string("should validate successfully when RobotService is available"), [=]() mutable
            {
                auto message = createMockMessage(std::string("test"));
                auto isValid = std::async([=]() { performScreenAction->validate(mockRuntime, message); });
                expect(isValid)->toBe(true);
                expect(mockRuntime->getService)->toHaveBeenCalledWith(std::string("ROBOT"));
            }
            );
            it(std::string("should fail validation when RobotService is not available"), [=]() mutable
            {
                auto runtimeWithoutService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn([=]() mutable
                    {
                        return nullptr;
                    }
                    )}
                }, mockRuntime)));
                auto message = createMockMessage(std::string("test"));
                auto isValid = std::async([=]() { performScreenAction->validate(runtimeWithoutService, message); });
                expect(isValid)->toBe(false);
            }
            );
        }
        );
        describe(std::string("handler"), [=]() mutable
        {
            shared message = createMockMessage(std::string("click submit button"));
            shared state = createMockState();
            it(std::string("should handle mouse move action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("x"), 100}, 
                        object::pair{std::string("y"), 200}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(100, 200);
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("thought"), std::string("Executed 1 screen actions successfully")}, 
                    object::pair{std::string("text"), std::string("Screen actions completed: moved mouse to (100, 200).")}
                });
            }
            );
            it(std::string("should handle click action with default button"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("left"), false);
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("thought"), std::string("Executed 1 screen actions successfully")}, 
                    object::pair{std::string("text"), std::string("Screen actions completed: clicked left mouse button.")}
                });
            }
            );
            it(std::string("should handle click action with specified button"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}, 
                        object::pair{std::string("button"), std::string("right")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("right"), false);
            }
            );
            it(std::string("should handle type action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}, 
                        object::pair{std::string("text"), std::string("Hello, World!")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std::string("Hello, World!"));
            }
            );
            it(std::string("should handle multiple actions in sequence"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("x"), 100}, 
                        object::pair{std::string("y"), 200}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}, 
                        object::pair{std::string("button"), std::string("left")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}, 
                        object::pair{std::string("text"), std::string("test input")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(100, 200);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("left"), false);
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std::string("test input"));
            }
            );
            it(std::string("should skip invalid move actions (missing coordinates)"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("x"), 100}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("y"), 200}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("x"), 50}, 
                        object::pair{std::string("y"), 75}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledTimes(1);
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(50, 75);
            }
            );
            it(std::string("should skip type actions without text"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}, 
                        object::pair{std::string("text"), string_empty}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}, 
                        object::pair{std::string("text"), std::string("valid text")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledTimes(1);
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std::string("valid text"));
            }
            );
            it(std::string("should handle empty steps array"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<any>()}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->not->toHaveBeenCalled();
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("thought"), std::string("No valid steps provided")}, 
                    object::pair{std::string("text"), std::string("Unable to perform screen action - no valid steps were provided.")}
                });
            }
            );
            it(std::string("should handle missing steps property"), [=]() mutable
            {
                auto options = object{};
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->not->toHaveBeenCalled();
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->not->toHaveBeenCalled();
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("thought"), std::string("No valid steps provided")}, 
                    object::pair{std::string("text"), std::string("Unable to perform screen action - no valid steps were provided.")}
                });
            }
            );
            it(std::string("should handle service unavailable gracefully"), [=]() mutable
            {
                auto runtimeWithoutService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn([=]() mutable
                    {
                        return nullptr;
                    }
                    )}
                }, mockRuntime)));
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(runtimeWithoutService, message, state, options, mockCallback); });
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("thought"), std::string("RobotService not available")}, 
                    object::pair{std::string("text"), std::string("Unable to perform screen action - robot service is not available.")}
                });
            }
            );
            it(std::string("should handle unknown action types gracefully"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<any>{ as<any>(object{
                        object::pair{std::string("action"), std::string("unknown_action")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("left"), false);
                expect(mockCallback)->toHaveBeenCalledWith(object{
                    object::pair{std::string("thought"), std::string("Executed 1 screen actions successfully")}, 
                    object::pair{std::string("text"), std::string("Screen actions completed: skipped invalid step: {"action":"unknown_action"}, clicked left mouse button.")}
                });
            }
            );
            it(std::string("should handle special characters in type action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}, 
                        object::pair{std::string("text"), std::string("Special chars: @#$%^&*()_+-=[]{}|;:,.<>?")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std::string("Special chars: @#$%^&*()_+-=[]{}|;:,.<>?"));
            }
            );
            it(std::string("should handle unicode characters in type action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("type")}, 
                        object::pair{std::string("text"), std::string("Unicode: 你好 🌟 café naïve")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::typeText, mockRobotService, std::placeholders::_1))->toHaveBeenCalledWith(std::string("Unicode: 你好 🌟 café naïve"));
            }
            );
            it(std::string("should handle negative coordinates in move action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("x"), -10}, 
                        object::pair{std::string("y"), -20}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(-10, -20);
            }
            );
            it(std::string("should handle large coordinates in move action"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("move")}, 
                        object::pair{std::string("x"), 9999}, 
                        object::pair{std::string("y"), 9999}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::moveMouse, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(9999, 9999);
            }
            );
            it(std::string("should handle all mouse button types"), [=]() mutable
            {
                auto options = object{
                    object::pair{std::string("steps"), array<std::shared_ptr<ScreenActionStep>>{ as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}, 
                        object::pair{std::string("button"), std::string("left")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}, 
                        object::pair{std::string("button"), std::string("right")}
                    }), as<std::shared_ptr<ScreenActionStep>>(object{
                        object::pair{std::string("action"), std::string("click")}, 
                        object::pair{std::string("button"), std::string("middle")}
                    }) }}
                };
                std::async([=]() { performScreenAction->handler(mockRuntime, message, state, options, mockCallback); });
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("left"), false);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("right"), false);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledWith(std::string("middle"), false);
                expect(std::bind(&RobotService::click, mockRobotService, std::placeholders::_1, std::placeholders::_2))->toHaveBeenCalledTimes(3);
            }
            );
        }
        );
    }
    );
}

MAIN
