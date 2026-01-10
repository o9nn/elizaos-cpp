#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/communityManager.test.h"

void Main(void)
{
    describe(std::string("CommunityManagerTestSuite"), [=]() mutable
    {
        shared<any> mockScenarioService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            mockScenarioService = object{
                object::pair{std::string("createWorld"), vi->fn()->mockResolvedValue(std::string("world-id"))}, 
                object::pair{std::string("createRoom"), vi->fn()->mockResolvedValue(std::string("room-id"))}, 
                object::pair{std::string("addParticipant"), vi->fn()->mockResolvedValue(true)}, 
                object::pair{std::string("sendMessage"), vi->fn()->mockResolvedValue(true)}, 
                object::pair{std::string("waitForCompletion"), vi->fn()->mockResolvedValue(true)}
            };
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
                object::pair{std::string("getService"), vi->fn()->mockReturnValue(mockScenarioService)}, 
                object::pair{std::string("agentId"), std::string("agent-id")}
            }));
        }
        );
        describe(std::string("Core Functionality"), [=]() mutable
        {
            it(std::string("should resolve conflicts"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Conflict Resolution");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Conflict Resolution" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std::string("Conflict Test"), std::string("Test Owner"));
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std::string("world-id"), std::string("general"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std::string("world-id"), std::string("room-id"), std::string("There's a user causing disruptions in the general channel"));
            }
            );
            it(std::string("should handle new user onboarding"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test New User Onboarding");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test New User Onboarding" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std::string("world-id"), std::string("welcome"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std::string("world-id"), std::string("room-id"), std::string("Hi everyone, I'm new here!"));
            }
            );
            it(std::string("should perform moderation actions"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Moderation Actions");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Moderation Actions" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std::string("Moderation Test"), std::string("Test Owner"));
                expect(mockScenarioService["waitForCompletion"])->toHaveBeenCalledWith(10000);
            }
            );
            it(std::string("should drive community engagement"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Community Engagement");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Community Engagement" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std::string("world-id"), std::string("room-id"), std::string("Let's plan the next community event"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should throw when missing scenario service"), [=]() mutable
            {
                auto brokenRuntime = utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime);
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Conflict Resolution");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std::string("Scenario service not found")); });
            }
            );
            it(std::string("should validate response timing"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test New User Onboarding");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std::string("Agent did not complete onboarding in time")); });
            }
            );
        }
        );
        describe(std::string("Character Compliance"), [=]() mutable
        {
            it(std::string("should ignore off-topic messages"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Community Engagement");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                auto messageContent = const_(const_(mockScenarioService["sendMessage"]["mock"]["calls"])[0])[3];
                expect(messageContent)->not->toContain(std::string("token price"));
                expect(messageContent)->not->toContain(std::string("marketing"));
            }
            );
            it(std::string("should maintain concise responses"), [=]() mutable
            {
                auto testSuite = std::make_shared<CommunityManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Conflict Resolution");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                auto messageCalls = mockScenarioService["sendMessage"]["mock"]["calls"];
                messageCalls["forEach"]([=](auto call) mutable
                {
                    auto message = const_(call)[3];
                    expect(message["split"](std::string(" "))["length"])->toBeLessThan(20);
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
