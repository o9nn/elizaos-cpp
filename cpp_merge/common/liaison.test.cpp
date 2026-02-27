#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/liaison.test.h"

void Main(void)
{
    describe(std:("LiaisonTestSuite"), [=]() mutable
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
            it(std:("should handle platform information requests"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Platform Information Request");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Platform Information Request" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std:("world-id"), std:("room-id"), expect->stringContaining(std:("Telegram group")));
            }
            );
            it(std:("should provide channel recommendations"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Channel Recommendation");
                }
                );
                mockScenarioService["sendMessage"]["mockImplementationOnce"]([=](auto _, auto __, auto ___, auto message) mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), string_empty + message + std:("\
Recommended channels: Discord #deployment, Slack #elizaos-development")}
                        }}
                    });
                }
                );
                auto response = std::async([=]() { test["fn"](mockRuntime); });
                auto messages = mockScenarioService["sendMessage"]["mock"]["calls"];
                expect(response["content"]["text"])->toContain(std:("Discord #deployment"));
            }
            );
        }
        );
        describe(std:("Cross-Platform Coordination"), [=]() mutable
        {
            it(std:("should handle cross-platform event setup"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Cross-Platform Coordination");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std:("world-id"), std:("room-id"), expect->stringContaining(std:("hackathon")));
            }
            );
            it(std:("should manage multi-platform announcements"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Announcement Management");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std:("world-id"), std:("room-id"), expect->stringContaining(std:("all platforms")));
            }
            );
        }
        );
        describe(std:("Message Filtering"), [=]() mutable
        {
            it(std:("should ignore off-topic messages"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Ignore Off-Topic");
                }
                );
                mockScenarioService["sendMessage"]["mockImplementationOnce"]([=]() mutable
                {
                    return Promise->resolve(nullptr);
                }
                );
                auto response = std::async([=]() { test["fn"](mockRuntime); });
                expect(response)->toBe(nullptr);
            }
            );
        }
        );
        describe(std:("Configuration Validation"), [=]() mutable
        {
            it(std:("should have correct plugins config"), [=]() mutable
            {
                expect(liaison->character->plugins)->toContain(std:("@elizaos/plugin-discord"));
            }
            );
            it(std:("should maintain liaison response style"), [=]() mutable
            {
                expect(liaison->character->style->all)->toContain(std:("Very short responses"));
                expect(liaison->character->style->chat)->toContain(std:("Focus on your job as a community liaison"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle missing scenario service"), [=]() mutable
            {
                auto brokenRuntime = utils::assign(object{
                    , 
                    object::pair{std:("getService"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime);
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Platform Information Request");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std:("Scenario service not found")); });
            }
            );
            it(std:("should handle response timeouts"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Channel Recommendation");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std:("Channel recommendation timed out")); });
            }
            );
        }
        );
    }
    );
}

MAIN
