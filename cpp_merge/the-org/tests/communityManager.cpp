#include "communityManager.test.h"

void Main(void)
{
    describe(std:("CommunityManagerTestSuite"), [=]() mutable
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
            it(std:("should resolve conflicts"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Conflict Resolution");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Conflict Resolution" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std:("Conflict Test"), std:("Test Owner"));
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std:("world-id"), std:("general"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std:("world-id"), std:("room-id"), std:("There's a user causing disruptions in the general channel"));
            }
            );
            it(std:("should handle new user onboarding"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test New User Onboarding");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test New User Onboarding" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std:("world-id"), std:("welcome"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std:("world-id"), std:("room-id"), std:("Hi everyone, I'm new here!"));
            }
            );
            it(std:("should perform moderation actions"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Moderation Actions");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Moderation Actions" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std:("Moderation Test"), std:("Test Owner"));
                expect(mockScenarioService["waitForCompletion"])->toHaveBeenCalledWith(10000);
            }
            );
            it(std:("should drive community engagement"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Community Engagement");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Community Engagement" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std:("world-id"), std:("room-id"), std:("Let's plan the next community event"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should throw when missing scenario service"), [=]() mutable
            {
                auto brokenRuntime = utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime);
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Conflict Resolution");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std:("Scenario service not found")); });
            }
            );
            it(std:("should validate response timing"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test New User Onboarding");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std:("Agent did not complete onboarding in time")); });
            }
            );
        }
        );
        describe(std:("Character Compliance"), [=]() mutable
        {
            it(std:("should ignore off-topic messages"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Community Engagement");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                auto messageContent = const_(const_(mockScenarioService["sendMessage"]["mock"]["calls"])[0])[3];
                expect(messageContent)->not->toContain(std:("token price"));
                expect(messageContent)->not->toContain(std:("marketing"));
            }
            );
            it(std:("should maintain concise responses"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Conflict Resolution");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                auto messageCalls = mockScenarioService["sendMessage"]["mock"]["calls"];
                messageCalls["forEach"]([=](auto call) mutable
                {
                    auto message = const_(call)[3];
                    expect(message["split"](std:(" "))["length"])->toBeLessThan(20);
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
