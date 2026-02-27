#include "actions.test.h"

void Main(void)
{
    dotenv->config();
    beforeAll([=]() mutable
    {
        spyOn(logger, std:("info"));
        spyOn(logger, std:("error"));
        spyOn(logger, std:("warn"));
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std:("Actions"), [=]() mutable
    {
        shared helloWorldAction = plugin->actions->find([=](auto action) mutable
        {
            return action["name"] == std:("HELLO_WORLD");
        }
        );
        it(std:("should pass core action tests"), [=]() mutable
        {
            if (plugin->actions) {
                auto coreTestResults = runCoreActionTests(plugin->actions);
                expect(coreTestResults)->toBeDefined();
                expect(coreTestResults["formattedNames"])->toBeDefined();
                expect(coreTestResults["formattedActions"])->toBeDefined();
                expect(coreTestResults["composedExamples"])->toBeDefined();
                documentTestResult(std:("Core Action Tests"), coreTestResults);
            }
        }
        );
        describe(std:("HELLO_WORLD Action"), [=]() mutable
        {
            it(std:("should exist in the plugin"), [=]() mutable
            {
                expect(helloWorldAction)->toBeDefined();
            }
            );
            it(std:("should have the correct structure"), [=]() mutable
            {
                if (helloWorldAction) {
                    expect(helloWorldAction)->toHaveProperty(std:("name"), std:("HELLO_WORLD"));
                    expect(helloWorldAction)->toHaveProperty(std:("description"));
                    expect(helloWorldAction)->toHaveProperty(std:("similes"));
                    expect(helloWorldAction)->toHaveProperty(std:("validate"));
                    expect(helloWorldAction)->toHaveProperty(std:("handler"));
                    expect(helloWorldAction)->toHaveProperty(std:("examples"));
                    expect(Array->isArray(helloWorldAction->similes))->toBe(true);
                    expect(Array->isArray(helloWorldAction->examples))->toBe(true);
                }
            }
            );
            it(std:("should have GREET and SAY_HELLO as similes"), [=]() mutable
            {
                if (helloWorldAction) {
                    expect(helloWorldAction->similes)->toContain(std:("GREET"));
                    expect(helloWorldAction->similes)->toContain(std:("SAY_HELLO"));
                }
            }
            );
            it(std:("should have at least one example"), [=]() mutable
            {
                if (AND((helloWorldAction), (helloWorldAction->examples))) {
                    expect(helloWorldAction->examples->length)->toBeGreaterThan(0);
                    auto firstExample = const_(helloWorldAction->examples)[0];
                    expect(firstExample->length)->toBeGreaterThan(1);
                    expect(const_(firstExample)[0])->toHaveProperty(std:("name"));
                    expect(const_(firstExample)[0])->toHaveProperty(std:("content"));
                    expect(const_(firstExample)[0]->content)->toHaveProperty(std:("text"));
                    expect(const_(firstExample)[0]->content->text)->toContain(std:("hello"));
                    expect(const_(firstExample)[1])->toHaveProperty(std:("name"));
                    expect(const_(firstExample)[1])->toHaveProperty(std:("content"));
                    expect(const_(firstExample)[1]->content)->toHaveProperty(std:("text"));
                    expect(const_(firstExample)[1]->content)->toHaveProperty(std:("actions"));
                    expect(const_(firstExample)[1]->content->text)->toBe(std:("hello world!"));
                    expect(const_(firstExample)[1]->content->actions)->toContain(std:("HELLO_WORLD"));
                }
            }
            );
            it(std:("should return true from validate function"), [=]() mutable
            {
                if (helloWorldAction) {
                    auto runtime = createMockRuntime();
                    auto mockMessage = createMockMessage(std:("Hello!"));
                    auto mockState = createMockState();
                    auto result = false;
                    auto error = nullptr;
                    try
                    {
                        result = std::async([=]() { helloWorldAction->validate(runtime, mockMessage, mockState); });
                        expect(result)->toBe(true);
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std:("Validate function error:"), e);
                    }
                    documentTestResult(std:("HELLO_WORLD action validate"), result, error);
                }
            }
            );
            it(std:("should call back with hello world response from handler"), [=]() mutable
            {
                if (helloWorldAction) {
                    auto runtime = createMockRuntime();
                    auto mockMessage = createMockMessage(std:("Hello!"));
                    auto mockState = createMockState();
                    shared callbackResponse = object{};
                    auto error = nullptr;
                    auto mockCallback = [=](auto response) mutable
                    {
                        callbackResponse = response;
                    };
                    try
                    {
                        std::async([=]() { helloWorldAction->handler(runtime, mockMessage, mockState, object{}, as<std::shared_ptr<HandlerCallback>>(mockCallback), array<any>()); });
                        expect(callbackResponse)->toBeTruthy();
                        expect(callbackResponse)->toHaveProperty(std:("text"));
                        expect(callbackResponse)->toHaveProperty(std:("actions"));
                        expect(callbackResponse["actions"])->toContain(std:("HELLO_WORLD"));
                        expect(callbackResponse)->toHaveProperty(std:("source"), std:("test"));
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std:("Handler function error:"), e);
                    }
                    documentTestResult(std:("HELLO_WORLD action handler"), callbackResponse, error);
                }
            }
            );
        }
        );
    }
    );
}

MAIN
