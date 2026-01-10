#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/liaison.test.h"

void Main(void)
{
    describe(std::string("LiaisonTestSuite"), [=]() mutable
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
            it(std::string("should handle platform information requests"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Platform Information Request");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Platform Information Request" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std::string("world-id"), std::string("room-id"), expect->stringContaining(std::string("Telegram group")));
            }
            );
            it(std::string("should provide channel recommendations"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Channel Recommendation");
                }
                );
                mockScenarioService["sendMessage"]["mockImplementationOnce"]([=](auto _, auto __, auto ___, auto message) mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), string_empty + message + std::string("\
Recommended channels: Discord #deployment, Slack #elizaos-development")}
                        }}
                    });
                }
                );
                auto response = std::async([=]() { test["fn"](mockRuntime); });
                auto messages = mockScenarioService["sendMessage"]["mock"]["calls"];
                expect(response["content"]["text"])->toContain(std::string("Discord #deployment"));
            }
            );
        }
        );
        describe(std::string("Cross-Platform Coordination"), [=]() mutable
        {
            it(std::string("should handle cross-platform event setup"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Cross-Platform Coordination");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std::string("world-id"), std::string("room-id"), expect->stringContaining(std::string("hackathon")));
            }
            );
            it(std::string("should manage multi-platform announcements"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Announcement Management");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std::string("world-id"), std::string("room-id"), expect->stringContaining(std::string("all platforms")));
            }
            );
        }
        );
        describe(std::string("Message Filtering"), [=]() mutable
        {
            it(std::string("should ignore off-topic messages"), [=]() mutable
            {
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Ignore Off-Topic");
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
        describe(std::string("Configuration Validation"), [=]() mutable
        {
            it(std::string("should have correct plugins config"), [=]() mutable
            {
                expect(liaison->character->plugins)->toContain(std::string("@elizaos/plugin-discord"));
            }
            );
            it(std::string("should maintain liaison response style"), [=]() mutable
            {
                expect(liaison->character->style->all)->toContain(std::string("Very short responses"));
                expect(liaison->character->style->chat)->toContain(std::string("Focus on your job as a community liaison"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle missing scenario service"), [=]() mutable
            {
                auto brokenRuntime = utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime);
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Platform Information Request");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std::string("Scenario service not found")); });
            }
            );
            it(std::string("should handle response timeouts"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<LiaisonTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Channel Recommendation");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std::string("Channel recommendation timed out")); });
            }
            );
        }
        );
    }
    );
}

MAIN
