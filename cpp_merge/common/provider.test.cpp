#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/provider.test.h"

object mockRobotService = object{
    object::pair{std:("moveMouse"), vi->fn()}, 
    object::pair{std:("click"), vi->fn()}, 
    object::pair{std:("typeText"), vi->fn()}, 
    object::pair{std:("getContext"), vi->fn()}, 
    object::pair{std:("updateContext"), vi->fn()}, 
    object::pair{std:("stop"), vi->fn()}, 
    object::pair{std:("capabilityDescription"), std:("Controls the screen and provides recent screen context with intelligent change detection and local OCR.")}
};
any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std:("getService"), vi->fn([=]() mutable
    {
        return as<std::shared_ptr<RobotService>>(as<any>(mockRobotService));
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
std::function<std::shared_ptr<ScreenContext>(std::shared_ptr<Partial<std::shared_ptr<ScreenContext>>>)> createMockScreenContext = [=](auto overrides = object{}) mutable
{
    return (utils::assign(object{
        object::pair{std:("screenshot"), Buffer::from(std:("mock-screenshot-data"))}, 
        object::pair{std:("currentDescription"), std:("A desktop with various windows and applications")}, 
        object::pair{std:("descriptionHistory"), array<any>()}, 
        object::pair{std:("ocr"), std:("Sample text from screen")}, 
        object::pair{std:("objects"), array<object>{ object{
            object::pair{std:("label"), std:("button")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 100}, 
                object::pair{std:("y"), 200}, 
                object::pair{std:("width"), 50}, 
                object::pair{std:("height"), 20}
            }}
        }, object{
            object::pair{std:("label"), std:("text_field")}, 
            object::pair{std:("bbox"), object{
                object::pair{std:("x"), 50}, 
                object::pair{std:("y"), 100}, 
                object::pair{std:("width"), 150}, 
                object::pair{std:("height"), 25}
            }}
        } }}, 
        object::pair{std:("timestamp"), Date->now()}, 
        object::pair{std:("changeDetected"), true}, 
        object::pair{std:("pixelDifferencePercentage"), 15.5}
    }, overrides));
};

void Main(void)
{
    describe(std:("screenProvider"), [=]() mutable
    {
        shared<std::shared_ptr<Memory>> message;
        shared<std::shared_ptr<State>> state;
        shared<std::shared_ptr<ScreenContext>> mockContext;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            message = createMockMessage(std:("get screen context"));
            state = createMockState();
            mockContext = createMockScreenContext();
            mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
        }
        );
        describe(std:("provider properties"), [=]() mutable
        {
            it(std:("should have correct provider properties"), [=]() mutable
            {
                expect(screenProvider->name)->toBe(std:("SCREEN_CONTEXT"));
                expect(screenProvider->description)->toBe(std:("Current screen context with OCR, description history, and change detection information."));
                expect(screenProvider->position)->toBe(50);
                expect(type_of(screenProvider->get))->toBe(std:("function"));
            }
            );
        }
        );
        describe(std:("get method"), [=]() mutable
        {
            it(std:("should return screen context when service is available"), [=]() mutable
            {
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(mockRobotService["getContext"])->toHaveBeenCalled();
                expect(result->values)->toEqual(object{
                    object::pair{std:("currentDescription"), mockContext->currentDescription}, 
                    object::pair{std:("ocr"), mockContext->ocr}, 
                    object::pair{std:("objects"), mockContext->objects}, 
                    object::pair{std:("changeDetected"), mockContext->changeDetected}, 
                    object::pair{std:("pixelDifferencePercentage"), mockContext->pixelDifferencePercentage}, 
                    object::pair{std:("historyCount"), mockContext->descriptionHistory->get_length()}, 
                    object::pair{std:("serviceStatus"), std:("active")}, 
                    object::pair{std:("dataAge"), expect->any(String)}, 
                    object::pair{std:("isStale"), false}
                });
                expect(result->text)->toContain(std:("# Current Screen Description"));
                expect(result->text)->toContain(mockContext->currentDescription);
                expect(result->text)->toContain(std:("# Text on Screen (OCR)"));
                expect(result->text)->toContain(mockContext->ocr);
                expect(result->text)->toContain(std:("# Interactive Objects"));
                expect(result->text)->toContain(std:("button at (100,200)"));
                expect(result->text)->toContain(std:("text_field at (50,100)"));
                expect(result->data)->toEqual(mockContext);
            }
            );
            it(std:("should handle service not available"), [=]() mutable
            {
                auto runtimeWithoutService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn([=]() mutable
                    {
                        return nullptr;
                    }
                    )}
                }, mockRuntime)));
                auto result = std::async([=]() { screenProvider->get(runtimeWithoutService, message, state); });
                expect(result->values)->toEqual(object{
                    object::pair{std:("serviceStatus"), std:("initializing")}, 
                    object::pair{std:("dataAge"), std:("unavailable")}, 
                    object::pair{std:("currentDescription"), string_empty}, 
                    object::pair{std:("ocr"), string_empty}, 
                    object::pair{std:("objects"), array<any>()}, 
                    object::pair{std:("changeDetected"), false}, 
                    object::pair{std:("pixelDifferencePercentage"), undefined}, 
                    object::pair{std:("historyCount"), 0}, 
                    object::pair{std:("isStale"), false}
                });
                expect(result->text)->toContain(std:("Robot Service Initializing"));
                expect(result->data)->toEqual(object{
                    object::pair{std:("serviceStatus"), std:("initializing")}
                });
            }
            );
            it(std:("should handle empty objects list"), [=]() mutable
            {
                mockContext->objects = array<any>();
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std:("# Interactive Objects"));
                expect(result->text)->toContain(std:("No object data available"));
                expect(result->values->objects)->toEqual(array<any>());
            }
            );
            it(std:("should handle multiple objects"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std:("label"), std:("button")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 100}, 
                        object::pair{std:("y"), 200}, 
                        object::pair{std:("width"), 50}, 
                        object::pair{std:("height"), 20}
                    }}
                }, object{
                    object::pair{std:("label"), std:("text_field")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 50}, 
                        object::pair{std:("y"), 100}, 
                        object::pair{std:("width"), 150}, 
                        object::pair{std:("height"), 25}
                    }}
                }, object{
                    object::pair{std:("label"), std:("image")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 300}, 
                        object::pair{std:("y"), 150}, 
                        object::pair{std:("width"), 100}, 
                        object::pair{std:("height"), 100}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std:("button at (100,200)"));
                expect(result->text)->toContain(std:("text_field at (50,100)"));
                expect(result->text)->toContain(std:("image at (300,150)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std:("should handle empty description"), [=]() mutable
            {
                mockContext->currentDescription = string_empty;
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values->currentDescription)->toBe(string_empty);
                expect(result->text)->toContain(std:("# Current Screen Description"));
                expect(result->text)->toContain(std:("No description available"));
            }
            );
            it(std:("should handle empty OCR"), [=]() mutable
            {
                mockContext->ocr = string_empty;
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values->ocr)->toBe(string_empty);
                expect(result->text)->toContain(std:("# Text on Screen (OCR)"));
                expect(result->text)->toContain(std:("No text detected"));
            }
            );
            it(std:("should handle service errors gracefully"), [=]() mutable
            {
                mockRobotService["getContext"] = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Service error")));
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values)->toEqual(object{
                    object::pair{std:("serviceStatus"), std:("processing")}, 
                    object::pair{std:("dataAge"), std:("processing")}, 
                    object::pair{std:("currentDescription"), string_empty}, 
                    object::pair{std:("ocr"), string_empty}, 
                    object::pair{std:("objects"), array<any>()}, 
                    object::pair{std:("changeDetected"), false}, 
                    object::pair{std:("pixelDifferencePercentage"), undefined}, 
                    object::pair{std:("historyCount"), 0}, 
                    object::pair{std:("isStale"), false}
                });
                expect(result->text)->toContain(std:("Processing Screen Data"));
                expect(result->data)->toEqual(object{
                    object::pair{std:("serviceStatus"), std:("processing")}
                });
            }
            );
            it(std:("should format text with proper headers"), [=]() mutable
            {
                mockContext->descriptionHistory = array<object>{ object{
                    object::pair{std:("description"), std:("Old screen")}, 
                    object::pair{std:("relativeTime"), std:("5 minutes ago")}, 
                    object::pair{std:("timestamp"), Date->now() - 300000}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                auto lines = result->text->split(std:("\
\
"));
                expect(const_(lines)[0])->toContain(std:("# Current Screen Description"));
                expect(const_(lines)[0])->toContain(mockContext->currentDescription);
                expect(const_(lines)[1])->toContain(std:("# Recent Screen History"));
                expect(const_(lines)[1])->toContain(std:("1. 5 minutes ago: Old screen"));
                expect(const_(lines)[2])->toContain(std:("# Text on Screen (OCR)"));
                expect(const_(lines)[2])->toContain(mockContext->ocr);
                expect(const_(lines)[3])->toContain(std:("# Interactive Objects"));
                expect(const_(lines)[3])->toContain(std:("button at (100,200)"));
                expect(const_(lines)[4])->toContain(std:("# Processing Status"));
                expect(const_(lines)[5])->toContain(std:("# Data Freshness"));
            }
            );
            it(std:("should handle objects with special characters in labels"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std:("label"), std:("button-submit")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 100}, 
                        object::pair{std:("y"), 200}, 
                        object::pair{std:("width"), 50}, 
                        object::pair{std:("height"), 20}
                    }}
                }, object{
                    object::pair{std:("label"), std:("text_field_email")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 50}, 
                        object::pair{std:("y"), 100}, 
                        object::pair{std:("width"), 150}, 
                        object::pair{std:("height"), 25}
                    }}
                }, object{
                    object::pair{std:("label"), std:("icon@2x")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 300}, 
                        object::pair{std:("y"), 150}, 
                        object::pair{std:("width"), 100}, 
                        object::pair{std:("height"), 100}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std:("button-submit at (100,200)"));
                expect(result->text)->toContain(std:("text_field_email at (50,100)"));
                expect(result->text)->toContain(std:("icon@2x at (300,150)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std:("should handle negative coordinates"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std:("label"), std:("off_screen_element")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), -10}, 
                        object::pair{std:("y"), -20}, 
                        object::pair{std:("width"), 50}, 
                        object::pair{std:("height"), 20}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std:("off_screen_element at (-10,-20)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std:("should handle large coordinates"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std:("label"), std:("large_screen_element")}, 
                    object::pair{std:("bbox"), object{
                        object::pair{std:("x"), 9999}, 
                        object::pair{std:("y"), 8888}, 
                        object::pair{std:("width"), 50}, 
                        object::pair{std:("height"), 20}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std:("large_screen_element at (9999,8888)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std:("should preserve all context data in result.data"), [=]() mutable
            {
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->data)->toEqual(mockContext);
                if (AND((result->data), (in(std:("screenshot"), result->data)))) {
                    expect((as<std::shared_ptr<ScreenContext>>(result->data))->screenshot)->toBeInstanceOf(Buffer);
                }
                if (AND((result->data), (in(std:("timestamp"), result->data)))) {
                    expect((as<std::shared_ptr<ScreenContext>>(result->data))->timestamp)->toBe(mockContext->timestamp);
                }
            }
            );
            it(std:("should handle unicode characters in description and OCR"), [=]() mutable
            {
                mockContext->currentDescription = std:("Desktop with 中文 characters and émojis 🌟");
                mockContext->ocr = std:("Text with ñoñó and café");
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values->currentDescription)->toBe(std:("Desktop with 中文 characters and émojis 🌟"));
                expect(result->values->ocr)->toBe(std:("Text with ñoñó and café"));
                expect(result->text)->toContain(std:("Desktop with 中文 characters and émojis 🌟"));
                expect(result->text)->toContain(std:("Text with ñoñó and café"));
            }
            );
        }
        );
    }
    );
}

MAIN
