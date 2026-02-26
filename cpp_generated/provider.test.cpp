#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-robot/tests/provider.test.h"

object mockRobotService = object{
    object::pair{std::string("moveMouse"), vi->fn()}, 
    object::pair{std::string("click"), vi->fn()}, 
    object::pair{std::string("typeText"), vi->fn()}, 
    object::pair{std::string("getContext"), vi->fn()}, 
    object::pair{std::string("updateContext"), vi->fn()}, 
    object::pair{std::string("stop"), vi->fn()}, 
    object::pair{std::string("capabilityDescription"), std::string("Controls the screen and provides recent screen context with intelligent change detection and local OCR.")}
};
any mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
    object::pair{std::string("getService"), vi->fn([=]() mutable
    {
        return as<std::shared_ptr<RobotService>>(as<any>(mockRobotService));
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
std::function<std::shared_ptr<ScreenContext>(std::shared_ptr<Partial<std::shared_ptr<ScreenContext>>>)> createMockScreenContext = [=](auto overrides = object{}) mutable
{
    return (utils::assign(object{
        object::pair{std::string("screenshot"), Buffer::from(std::string("mock-screenshot-data"))}, 
        object::pair{std::string("currentDescription"), std::string("A desktop with various windows and applications")}, 
        object::pair{std::string("descriptionHistory"), array<any>()}, 
        object::pair{std::string("ocr"), std::string("Sample text from screen")}, 
        object::pair{std::string("objects"), array<object>{ object{
            object::pair{std::string("label"), std::string("button")}, 
            object::pair{std::string("bbox"), object{
                object::pair{std::string("x"), 100}, 
                object::pair{std::string("y"), 200}, 
                object::pair{std::string("width"), 50}, 
                object::pair{std::string("height"), 20}
            }}
        }, object{
            object::pair{std::string("label"), std::string("text_field")}, 
            object::pair{std::string("bbox"), object{
                object::pair{std::string("x"), 50}, 
                object::pair{std::string("y"), 100}, 
                object::pair{std::string("width"), 150}, 
                object::pair{std::string("height"), 25}
            }}
        } }}, 
        object::pair{std::string("timestamp"), Date->now()}, 
        object::pair{std::string("changeDetected"), true}, 
        object::pair{std::string("pixelDifferencePercentage"), 15.5}
    }, overrides));
};

void Main(void)
{
    describe(std::string("screenProvider"), [=]() mutable
    {
        shared<std::shared_ptr<Memory>> message;
        shared<std::shared_ptr<State>> state;
        shared<std::shared_ptr<ScreenContext>> mockContext;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            message = createMockMessage(std::string("get screen context"));
            state = createMockState();
            mockContext = createMockScreenContext();
            mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
        }
        );
        describe(std::string("provider properties"), [=]() mutable
        {
            it(std::string("should have correct provider properties"), [=]() mutable
            {
                expect(screenProvider->name)->toBe(std::string("SCREEN_CONTEXT"));
                expect(screenProvider->description)->toBe(std::string("Current screen context with OCR, description history, and change detection information."));
                expect(screenProvider->position)->toBe(50);
                expect(type_of(screenProvider->get))->toBe(std::string("function"));
            }
            );
        }
        );
        describe(std::string("get method"), [=]() mutable
        {
            it(std::string("should return screen context when service is available"), [=]() mutable
            {
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(mockRobotService["getContext"])->toHaveBeenCalled();
                expect(result->values)->toEqual(object{
                    object::pair{std::string("currentDescription"), mockContext->currentDescription}, 
                    object::pair{std::string("ocr"), mockContext->ocr}, 
                    object::pair{std::string("objects"), mockContext->objects}, 
                    object::pair{std::string("changeDetected"), mockContext->changeDetected}, 
                    object::pair{std::string("pixelDifferencePercentage"), mockContext->pixelDifferencePercentage}, 
                    object::pair{std::string("historyCount"), mockContext->descriptionHistory->get_length()}, 
                    object::pair{std::string("serviceStatus"), std::string("active")}, 
                    object::pair{std::string("dataAge"), expect->any(String)}, 
                    object::pair{std::string("isStale"), false}
                });
                expect(result->text)->toContain(std::string("# Current Screen Description"));
                expect(result->text)->toContain(mockContext->currentDescription);
                expect(result->text)->toContain(std::string("# Text on Screen (OCR)"));
                expect(result->text)->toContain(mockContext->ocr);
                expect(result->text)->toContain(std::string("# Interactive Objects"));
                expect(result->text)->toContain(std::string("button at (100,200)"));
                expect(result->text)->toContain(std::string("text_field at (50,100)"));
                expect(result->data)->toEqual(mockContext);
            }
            );
            it(std::string("should handle service not available"), [=]() mutable
            {
                auto runtimeWithoutService = as<std::shared_ptr<IAgentRuntime>>(as<any>(utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn([=]() mutable
                    {
                        return nullptr;
                    }
                    )}
                }, mockRuntime)));
                auto result = std::async([=]() { screenProvider->get(runtimeWithoutService, message, state); });
                expect(result->values)->toEqual(object{
                    object::pair{std::string("serviceStatus"), std::string("initializing")}, 
                    object::pair{std::string("dataAge"), std::string("unavailable")}, 
                    object::pair{std::string("currentDescription"), string_empty}, 
                    object::pair{std::string("ocr"), string_empty}, 
                    object::pair{std::string("objects"), array<any>()}, 
                    object::pair{std::string("changeDetected"), false}, 
                    object::pair{std::string("pixelDifferencePercentage"), undefined}, 
                    object::pair{std::string("historyCount"), 0}, 
                    object::pair{std::string("isStale"), false}
                });
                expect(result->text)->toContain(std::string("Robot Service Initializing"));
                expect(result->data)->toEqual(object{
                    object::pair{std::string("serviceStatus"), std::string("initializing")}
                });
            }
            );
            it(std::string("should handle empty objects list"), [=]() mutable
            {
                mockContext->objects = array<any>();
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std::string("# Interactive Objects"));
                expect(result->text)->toContain(std::string("No object data available"));
                expect(result->values->objects)->toEqual(array<any>());
            }
            );
            it(std::string("should handle multiple objects"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std::string("label"), std::string("button")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 100}, 
                        object::pair{std::string("y"), 200}, 
                        object::pair{std::string("width"), 50}, 
                        object::pair{std::string("height"), 20}
                    }}
                }, object{
                    object::pair{std::string("label"), std::string("text_field")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 50}, 
                        object::pair{std::string("y"), 100}, 
                        object::pair{std::string("width"), 150}, 
                        object::pair{std::string("height"), 25}
                    }}
                }, object{
                    object::pair{std::string("label"), std::string("image")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 300}, 
                        object::pair{std::string("y"), 150}, 
                        object::pair{std::string("width"), 100}, 
                        object::pair{std::string("height"), 100}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std::string("button at (100,200)"));
                expect(result->text)->toContain(std::string("text_field at (50,100)"));
                expect(result->text)->toContain(std::string("image at (300,150)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std::string("should handle empty description"), [=]() mutable
            {
                mockContext->currentDescription = string_empty;
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values->currentDescription)->toBe(string_empty);
                expect(result->text)->toContain(std::string("# Current Screen Description"));
                expect(result->text)->toContain(std::string("No description available"));
            }
            );
            it(std::string("should handle empty OCR"), [=]() mutable
            {
                mockContext->ocr = string_empty;
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values->ocr)->toBe(string_empty);
                expect(result->text)->toContain(std::string("# Text on Screen (OCR)"));
                expect(result->text)->toContain(std::string("No text detected"));
            }
            );
            it(std::string("should handle service errors gracefully"), [=]() mutable
            {
                mockRobotService["getContext"] = vi->fn()->mockRejectedValue(std::make_shared<Error>(std::string("Service error")));
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values)->toEqual(object{
                    object::pair{std::string("serviceStatus"), std::string("processing")}, 
                    object::pair{std::string("dataAge"), std::string("processing")}, 
                    object::pair{std::string("currentDescription"), string_empty}, 
                    object::pair{std::string("ocr"), string_empty}, 
                    object::pair{std::string("objects"), array<any>()}, 
                    object::pair{std::string("changeDetected"), false}, 
                    object::pair{std::string("pixelDifferencePercentage"), undefined}, 
                    object::pair{std::string("historyCount"), 0}, 
                    object::pair{std::string("isStale"), false}
                });
                expect(result->text)->toContain(std::string("Processing Screen Data"));
                expect(result->data)->toEqual(object{
                    object::pair{std::string("serviceStatus"), std::string("processing")}
                });
            }
            );
            it(std::string("should format text with proper headers"), [=]() mutable
            {
                mockContext->descriptionHistory = array<object>{ object{
                    object::pair{std::string("description"), std::string("Old screen")}, 
                    object::pair{std::string("relativeTime"), std::string("5 minutes ago")}, 
                    object::pair{std::string("timestamp"), Date->now() - 300000}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                auto lines = result->text->split(std::string("\
\
"));
                expect(const_(lines)[0])->toContain(std::string("# Current Screen Description"));
                expect(const_(lines)[0])->toContain(mockContext->currentDescription);
                expect(const_(lines)[1])->toContain(std::string("# Recent Screen History"));
                expect(const_(lines)[1])->toContain(std::string("1. 5 minutes ago: Old screen"));
                expect(const_(lines)[2])->toContain(std::string("# Text on Screen (OCR)"));
                expect(const_(lines)[2])->toContain(mockContext->ocr);
                expect(const_(lines)[3])->toContain(std::string("# Interactive Objects"));
                expect(const_(lines)[3])->toContain(std::string("button at (100,200)"));
                expect(const_(lines)[4])->toContain(std::string("# Processing Status"));
                expect(const_(lines)[5])->toContain(std::string("# Data Freshness"));
            }
            );
            it(std::string("should handle objects with special characters in labels"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std::string("label"), std::string("button-submit")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 100}, 
                        object::pair{std::string("y"), 200}, 
                        object::pair{std::string("width"), 50}, 
                        object::pair{std::string("height"), 20}
                    }}
                }, object{
                    object::pair{std::string("label"), std::string("text_field_email")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 50}, 
                        object::pair{std::string("y"), 100}, 
                        object::pair{std::string("width"), 150}, 
                        object::pair{std::string("height"), 25}
                    }}
                }, object{
                    object::pair{std::string("label"), std::string("icon@2x")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 300}, 
                        object::pair{std::string("y"), 150}, 
                        object::pair{std::string("width"), 100}, 
                        object::pair{std::string("height"), 100}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std::string("button-submit at (100,200)"));
                expect(result->text)->toContain(std::string("text_field_email at (50,100)"));
                expect(result->text)->toContain(std::string("icon@2x at (300,150)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std::string("should handle negative coordinates"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std::string("label"), std::string("off_screen_element")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), -10}, 
                        object::pair{std::string("y"), -20}, 
                        object::pair{std::string("width"), 50}, 
                        object::pair{std::string("height"), 20}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std::string("off_screen_element at (-10,-20)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std::string("should handle large coordinates"), [=]() mutable
            {
                mockContext->objects = array<object>{ object{
                    object::pair{std::string("label"), std::string("large_screen_element")}, 
                    object::pair{std::string("bbox"), object{
                        object::pair{std::string("x"), 9999}, 
                        object::pair{std::string("y"), 8888}, 
                        object::pair{std::string("width"), 50}, 
                        object::pair{std::string("height"), 20}
                    }}
                } };
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->text)->toContain(std::string("large_screen_element at (9999,8888)"));
                expect(result->values->objects)->toEqual(mockContext->objects);
            }
            );
            it(std::string("should preserve all context data in result.data"), [=]() mutable
            {
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->data)->toEqual(mockContext);
                if (AND((result->data), (in(std::string("screenshot"), result->data)))) {
                    expect((as<std::shared_ptr<ScreenContext>>(result->data))->screenshot)->toBeInstanceOf(Buffer);
                }
                if (AND((result->data), (in(std::string("timestamp"), result->data)))) {
                    expect((as<std::shared_ptr<ScreenContext>>(result->data))->timestamp)->toBe(mockContext->timestamp);
                }
            }
            );
            it(std::string("should handle unicode characters in description and OCR"), [=]() mutable
            {
                mockContext->currentDescription = std::string("Desktop with 中文 characters and émojis 🌟");
                mockContext->ocr = std::string("Text with ñoñó and café");
                mockRobotService["getContext"] = vi->fn()->mockResolvedValue(mockContext);
                auto result = std::async([=]() { screenProvider->get(mockRuntime, message, state); });
                expect(result->values->currentDescription)->toBe(std::string("Desktop with 中文 characters and émojis 🌟"));
                expect(result->values->ocr)->toBe(std::string("Text with ñoñó and café"));
                expect(result->text)->toContain(std::string("Desktop with 中文 characters and émojis 🌟"));
                expect(result->text)->toContain(std::string("Text with ñoñó and café"));
            }
            );
        }
        );
    }
    );
}

MAIN
