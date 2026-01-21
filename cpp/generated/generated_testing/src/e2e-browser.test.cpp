#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-stagehand/src/__tests__/e2e-browser.test.h"

std::function<any()> createMockRuntime = [=]() mutable
{
    return as<std::shared_ptr<IAgentRuntime>>(as<any>((object{
        object::pair{std::string("getSetting"), [=](auto key) mutable
        {
            if (OR((key == std::string("ENABLE_BROWSER")), (key == std::string("BROWSER_ENABLED")))) {
                return std::string("true");
            }
            return undefined;
        }
        }, 
        object::pair{std::string("getService"), [=](auto serviceName) mutable
        {
            if (serviceName == StagehandService::serviceType) {
                return service;
            }
            return nullptr;
        }
        }, 
        object::pair{std::string("registerService"), mock()}
    })));
};
std::function<any(string)> createMemory = [=](auto text) mutable
{
    return as<std::shared_ptr<Memory>>((object{
        object::pair{std::string("id"), std::string("mem-") + Date->now() + string_empty}, 
        object::pair{std::string("userId"), std::string("test-user")}, 
        object::pair{std::string("agentId"), std::string("test-agent")}, 
        object::pair{std::string("roomId"), std::string("test-room")}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), std::string("text")}
        }}, 
        object::pair{std::string("createdAt"), std::make_shared<Date>()}
    }));
};
std::shared_ptr<StagehandService> service;
std::shared_ptr<IAgentRuntime> runtime;

void Main(void)
{
    describe(std::string("E2E Browser Actions"), [=]() mutable
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
        it(std::string("should navigate to a URL"), [=]() mutable
        {
            auto navigateAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_NAVIGATE");
            }
            );
            expect(navigateAction)->toBeDefined();
            auto message = createMemory(std::string("Navigate to https://example.com"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canNavigate = std::async([=]() { navigateAction->validate(runtime, message, state); });
            expect(canNavigate)->toBe(true);
            auto mockCallback = mock();
            auto result = std::async([=]() { navigateAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std::string("navigated to https://example.com"));
            expect(callbackArgs->actions)->toContain(std::string("BROWSER_NAVIGATE"));
            expect(result)->toBeDefined();
            expect(result->success)->toBe(true);
            expect(result->data->url)->toBe(std::string("https://example.com"));
        }
        , 30000);
        it(std::string("should click on an element"), [=]() mutable
        {
            auto clickAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_CLICK");
            }
            );
            expect(clickAction)->toBeDefined();
            auto message = createMemory(std::string("Click on the submit button"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canClick = std::async([=]() { clickAction->validate(runtime, message, state); });
            expect(canClick)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { clickAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std::string("clicked on "the submit button""));
            expect(callbackArgs->actions)->toContain(std::string("BROWSER_CLICK"));
        }
        , 30000);
        it(std::string("should type text into a field"), [=]() mutable
        {
            auto typeAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_TYPE");
            }
            );
            expect(typeAction)->toBeDefined();
            auto message = createMemory(std::string("Type "hello world" in the search box"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canType = std::async([=]() { typeAction->validate(runtime, message, state); });
            expect(canType)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { typeAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std::string("typed "hello world" in the search box"));
            expect(callbackArgs->actions)->toContain(std::string("BROWSER_TYPE"));
        }
        , 30000);
        it(std::string("should select an option from dropdown"), [=]() mutable
        {
            auto selectAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_SELECT");
            }
            );
            expect(selectAction)->toBeDefined();
            auto message = createMemory(std::string("Select "United States" from the country dropdown"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canSelect = std::async([=]() { selectAction->validate(runtime, message, state); });
            expect(canSelect)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { selectAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std::string("selected "United States" from the country dropdown"));
            expect(callbackArgs->actions)->toContain(std::string("BROWSER_SELECT"));
        }
        , 30000);
        it(std::string("should extract data from page"), [=]() mutable
        {
            auto extractAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_EXTRACT");
            }
            );
            expect(extractAction)->toBeDefined();
            auto message = createMemory(std::string("Extract the main heading from the page"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canExtract = std::async([=]() { extractAction->validate(runtime, message, state); });
            expect(canExtract)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { extractAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toMatch((new RegExp(std::string("found the main heading|couldn't fin"))));
            expect(callbackArgs->actions)->toContain(std::string("BROWSER_EXTRACT"));
        }
        , 30000);
        it(std::string("should take a screenshot"), [=]() mutable
        {
            auto screenshotAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_SCREENSHOT");
            }
            );
            expect(screenshotAction)->toBeDefined();
            auto message = createMemory(std::string("Take a screenshot of the page"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto canScreenshot = std::async([=]() { screenshotAction->validate(runtime, message, state); });
            expect(canScreenshot)->toBe(true);
            auto mockCallback = mock();
            std::async([=]() { screenshotAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->text)->toContain(std::string("taken a screenshot"));
            expect(callbackArgs->actions)->toContain(std::string("BROWSER_SCREENSHOT"));
            expect(callbackArgs->data)->toBeDefined();
            expect(callbackArgs->data->mimeType)->toBe(std::string("image/png"));
        }
        , 30000);
        it(std::string("should handle navigation errors gracefully"), [=]() mutable
        {
            auto navigateAction = stagehandPlugin->actions->find([=](auto a) mutable
            {
                return a["name"] == std::string("BROWSER_NAVIGATE");
            }
            );
            expect(navigateAction)->toBeDefined();
            auto message = createMemory(std::string("Navigate to invalid://not-a-url"));
            auto state = as<std::shared_ptr<State>>(object{});
            auto mockCallback = mock();
            auto result = std::async([=]() { navigateAction->handler(runtime, message, state, object{}, mockCallback); });
            expect(mockCallback)->toHaveBeenCalled();
            auto callbackArgs = const_(const_(mockCallback->mock->calls)[0])[0];
            expect(callbackArgs->error)->toBe(true);
            expect(callbackArgs->text)->toContain(std::string("Security error"));
            expect(result->success)->toBe(false);
        }
        , 30000);
    }
    );
}

MAIN
