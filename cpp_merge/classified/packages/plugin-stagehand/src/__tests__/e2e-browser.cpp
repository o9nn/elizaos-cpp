#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/e2e-browser.test.h"

std::function<any()> createMockRuntime = [=]() mutable
{
    return as<std::shared_ptr<IAgentRuntime>>(as<any>((object{
        object::pair{std:("getSetting"), [=](auto key) mutable
        {
            if (OR((key == std:("ENABLE_BROWSER")), (key == std:("BROWSER_ENABLED")))) {
                return std:("true");
            }
            return undefined;
        }
        }, 
        object::pair{std:("getService"), [=](auto serviceName) mutable
        {
            if (serviceName == StagehandService::serviceType) {
                return service;
            }
            return nullptr;
        }
        }, 
        object::pair{std:("registerService"), mock()}
    })));
};
std::function<any(string)> createMemory = [=](auto text) mutable
{
    return as<std::shared_ptr<Memory>>((object{
        object::pair{std:("id"), std:("mem-") + Date->now() + string_empty}, 
        object::pair{std:("userId"), std:("test-user")}, 
        object::pair{std:("agentId"), std:("test-agent")}, 
        object::pair{std:("roomId"), std:("test-room")}, 
        object::pair{std:("content"), object{
            object::pair{std:("text"), std:("text")}
        }}, 
        object::pair{std:("createdAt"), std::make_shared<Date>()}
    }));
};
std::shared_ptr<StagehandService> service;
std::shared_ptr<IAgentRuntime> runtime;

void Main(void)
{
    describe(std:("E2E Browser Actions"), [=]() mutable
    {
        beforeAll([=]() mutable
        {
            runtime = createMockRuntime();
            service = std::make_shared<StagehandService>();
            std::async([=]() { service->init(runtime); });
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 3000);
            }
            ); });
        }
        , 30000);
        afterAll([=]() mutable
        {
            if (service) {
                std::async([=]() { service->stop(); });
            }
        }
        , 10000);
        it(std:("should navigate to a URL"), [=]() mutable
        {
            auto navigateAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_NAVIGATE");
            }
            );
            expect(navigateAction)->toBeDefined();
            auto message = createMemory(std:("Navigate to https://example.com"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canNavigate = std::async([=]() { navigateAction->validate(runtime, message, state); });
            expect(canNavigate)->toBe(true);
            auto mockCallback = mock();
            auto result = std::async([=]() { navigateAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std:("navigated to https://example.com"));
            expect(callbackArgs->actions)->toContain(std:("BROWSER_NAVIGATE"));
            expect(result)->toBeDefined();
            expect(result->success)->toBe(true);
            expect(result->data->url)->toBe(std:("https://example.com"));
        }
        , 30000);
        it(std:("should click on an element"), [=]() mutable
        {
            auto clickAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_CLICK");
            }
            );
            expect(clickAction)->toBeDefined();
            auto message = createMemory(std:("Click on the submit button"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canClick = std::async([=]() { clickAction->validate(runtime, message, state); });
            expect(canClick)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { clickAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std:("clicked on "the submit button""));
            expect(callbackArgs->actions)->toContain(std:("BROWSER_CLICK"));
        }
        , 30000);
        it(std:("should type text into a field"), [=]() mutable
        {
            auto typeAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_TYPE");
            }
            );
            expect(typeAction)->toBeDefined();
            auto message = createMemory(std:("Type "hello world" in the search box"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canType = std::async([=]() { typeAction->validate(runtime, message, state); });
            expect(canType)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { typeAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std:("typed "hello world" in the search box"));
            expect(callbackArgs->actions)->toContain(std:("BROWSER_TYPE"));
        }
        , 30000);
        it(std:("should select an option from dropdown"), [=]() mutable
        {
            auto selectAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_SELECT");
            }
            );
            expect(selectAction)->toBeDefined();
            auto message = createMemory(std:("Select "United States" from the country dropdown"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canSelect = std::async([=]() { selectAction->validate(runtime, message, state); });
            expect(canSelect)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { selectAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std:("selected "United States" from the country dropdown"));
            expect(callbackArgs->actions)->toContain(std:("BROWSER_SELECT"));
        }
        , 30000);
        it(std:("should extract data from page"), [=]() mutable
        {
            auto extractAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_EXTRACT");
            }
            );
            expect(extractAction)->toBeDefined();
            auto message = createMemory(std:("Extract the main heading from the page"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canExtract = std::async([=]() { extractAction->validate(runtime, message, state); });
            expect(canExtract)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { extractAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toMatch((new RegExp(std:("found the main heading|couldn't fin"))));
            expect(callbackArgs->actions)->toContain(std:("BROWSER_EXTRACT"));
        }
        , 30000);
        it(std:("should take a screenshot"), [=]() mutable
        {
            auto screenshotAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_SCREENSHOT");
            }
            );
            expect(screenshotAction)->toBeDefined();
            auto message = createMemory(std:("Take a screenshot of the page"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canScreenshot = std::async([=]() { screenshotAction->validate(runtime, message, state); });
            expect(canScreenshot)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { screenshotAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std:("taken a screenshot"));
            expect(callbackArgs->actions)->toContain(std:("BROWSER_SCREENSHOT"));
            expect(callbackArgs->data)->toBeDefined();
            expect(callbackArgs->data->mimeType)->toBe(std:("image/png"));
        }
        , 30000);
        it(std:("should handle navigation errors gracefully"), [=]() mutable
        {
            auto navigateAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std:("BROWSER_NAVIGATE");
            }
            );
            expect(navigateAction)->toBeDefined();
            auto message = createMemory(std:("Navigate to invalid://not-a-url"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto mockCallback = mock();
            auto result = std::async([=]() { navigateAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->error)->toBe(true);
            expect(callbackArgs->text)->toContain(std:("Security error"));
            expect(result->success)->toBe(false);
        }
        , 30000);
    }
    );
}

MAIN
