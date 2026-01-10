#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/actions.test.h"

void Main(void)
{
    dotenv->config();
    beforeAll([=]() mutable
    {
        spyOn(logger, std::string("info"));
        spyOn(logger, std::string("error"));
        spyOn(logger, std::string("warn"));
    }
    );
    afterAll([=]() mutable
    {
    }
    );
    describe(std::string("Actions"), [=]() mutable
    {
        shared helloWorldAction = plugin->actions->find([=](auto action) mutable
        {
            return action["name"] == std::string("HELLO_WORLD");
        }
        );
        it(std::string("should pass core action tests"), [=]() mutable
        {
            if (plugin->actions) {
                auto coreTestResults = runCoreActionTests(plugin->actions);
                expect(coreTestResults)->toBeDefined();
                expect(coreTestResults["formattedNames"])->toBeDefined();
                expect(coreTestResults["formattedActions"])->toBeDefined();
                expect(coreTestResults["composedExamples"])->toBeDefined();
                documentTestResult(std::string("Core Action Tests"), coreTestResults);
            }
        }
        );
        describe(std::string("HELLO_WORLD Action"), [=]() mutable
        {
            it(std::string("should exist in the plugin"), [=]() mutable
            {
                expect(helloWorldAction)->toBeDefined();
            }
            );
            it(std::string("should have the correct structure"), [=]() mutable
            {
                if (helloWorldAction) {
                    expect(helloWorldAction)->toHaveProperty(std::string("name"), std::string("HELLO_WORLD"));
                    expect(helloWorldAction)->toHaveProperty(std::string("description"));
                    expect(helloWorldAction)->toHaveProperty(std::string("similes"));
                    expect(helloWorldAction)->toHaveProperty(std::string("validate"));
                    expect(helloWorldAction)->toHaveProperty(std::string("handler"));
                    expect(helloWorldAction)->toHaveProperty(std::string("examples"));
                    expect(Array->isArray(helloWorldAction->similes))->toBe(true);
                    expect(Array->isArray(helloWorldAction->examples))->toBe(true);
                }
            }
            );
            it(std::string("should have GREET and SAY_HELLO as similes"), [=]() mutable
            {
                if (helloWorldAction) {
                    expect(helloWorldAction->similes)->toContain(std::string("GREET"));
                    expect(helloWorldAction->similes)->toContain(std::string("SAY_HELLO"));
                }
            }
            );
            it(std::string("should have at least one example"), [=]() mutable
            {
                if (AND((helloWorldAction), (helloWorldAction->examples))) {
                    expect(helloWorldAction->examples->length)->toBeGreaterThan(0);
                    auto firstExample = const_(helloWorldAction->examples)[0];
                    expect(firstExample->length)->toBeGreaterThan(1);
                    expect(const_(firstExample)[0])->toHaveProperty(std::string("name"));
                    expect(const_(firstExample)[0])->toHaveProperty(std::string("content"));
                    expect(const_(firstExample)[0]->content)->toHaveProperty(std::string("text"));
                    expect(const_(firstExample)[0]->content->text)->toContain(std::string("hello"));
                    expect(const_(firstExample)[1])->toHaveProperty(std::string("name"));
                    expect(const_(firstExample)[1])->toHaveProperty(std::string("content"));
                    expect(const_(firstExample)[1]->content)->toHaveProperty(std::string("text"));
                    expect(const_(firstExample)[1]->content)->toHaveProperty(std::string("actions"));
                    expect(const_(firstExample)[1]->content->text)->toBe(std::string("hello world!"));
                    expect(const_(firstExample)[1]->content->actions)->toContain(std::string("HELLO_WORLD"));
                }
            }
            );
            it(std::string("should return true from validate function"), [=]() mutable
            {
                if (helloWorldAction) {
                    auto runtime = createMockRuntime();
                    auto mockMessage = createMockMessage(std::string("Hello!"));
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
                        logger->error(std::string("Validate function error:"), e);
                    }
                    documentTestResult(std::string("HELLO_WORLD action validate"), result, error);
                }
            }
            );
            it(std::string("should call back with hello world response from handler"), [=]() mutable
            {
                if (helloWorldAction) {
                    auto runtime = createMockRuntime();
                    auto mockMessage = createMockMessage(std::string("Hello!"));
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
                        expect(callbackResponse)->toHaveProperty(std::string("text"));
                        expect(callbackResponse)->toHaveProperty(std::string("actions"));
                        expect(callbackResponse["actions"])->toContain(std::string("HELLO_WORLD"));
                        expect(callbackResponse)->toHaveProperty(std::string("source"), std::string("test"));
                    }
                    catch (const any& e)
                    {
                        error = as<std::shared_ptr<Error>>(e);
                        logger->error(std::string("Handler function error:"), e);
                    }
                    documentTestResult(std::string("HELLO_WORLD action handler"), callbackResponse, error);
                }
            }
            );
        }
        );
    }
    );
}

MAIN
