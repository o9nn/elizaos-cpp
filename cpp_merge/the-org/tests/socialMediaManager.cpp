#include "socialMediaManager.test.h"

void Main(void)
{
    describe(std:("SocialMediaManagerTestSuite"), [=]() mutable
    {
        shared<any> mockScenarioService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockScenarioService = object{
                object::pair{std:("createWorld"), vi->fn()->mockResolvedValue(std:("world-id"))}, 
                object::pair{std:("createRoom"), vi->fn()->mockResolvedValue(std:("room-id"))}, 
                object::pair{std:("addParticipant"), vi->fn()->mockResolvedValue(true)}, 
                object::pair{std:("sendMessage"), vi->fn()->mockResolvedValue(true)}, 
                object::pair{std:("waitForCompletion"), vi->fn()->mockResolvedValue(true)}
            };
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std:("getService"), vi->fn()->mockReturnValue(mockScenarioService)}, 
                object::pair{std:("agentId"), std:("agent-id")}
            }));
        }
        );
        describe(std:("Core Functionality"), [=]() mutable
        {
            it(std:("should complete onboarding process successfully"), [=]() mutable
            {
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Onboarding Process");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Onboarding Process" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std:("Test Organization"), std:("Test Owner"));
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std:("world-id"), std:("general"));
                expect(mockScenarioService["addParticipant"])->toHaveBeenCalledTimes(2);
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalled();
            }
            );
            it(std:("should handle cross-platform post creation"), [=]() mutable
            {
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Cross-Platform Post Creation");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std:("world-id"), std:("room-id"), std:("Please create a post about our new product launch for Twitter and Discord"));
                expect(mockScenarioService["waitForCompletion"])->toHaveBeenCalledWith(10000);
            }
            );
            it(std:("should manage multiple user queries"), [=]() mutable
            {
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Response to User Queries");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledTimes(3);
                expect(mockScenarioService["waitForCompletion"])->toHaveBeenCalledTimes(3);
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should throw error when missing scenario service"), [=]() mutable
            {
                auto brokenRuntime = utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime);
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Onboarding Process");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std:("Scenario service not found")); });
            }
            );
            it(std:("should handle operation timeouts"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Onboarding Process");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std:("Agent did not complete onboarding response in time")); });
            }
            );
        }
        );
    }
    );
}

MAIN
