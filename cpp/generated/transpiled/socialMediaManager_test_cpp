#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/socialMediaManager.test.h"

void Main(void)
{
    describe(std::string("SocialMediaManagerTestSuite"), [=]() mutable
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
            it(std::string("should complete onboarding process successfully"), [=]() mutable
            {
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Onboarding Process");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Onboarding Process" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std::string("Test Organization"), std::string("Test Owner"));
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std::string("world-id"), std::string("general"));
                expect(mockScenarioService["addParticipant"])->toHaveBeenCalledTimes(2);
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalled();
            }
            );
            it(std::string("should handle cross-platform post creation"), [=]() mutable
            {
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Cross-Platform Post Creation");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std::string("world-id"), std::string("room-id"), std::string("Please create a post about our new product launch for Twitter and Discord"));
                expect(mockScenarioService["waitForCompletion"])->toHaveBeenCalledWith(10000);
            }
            );
            it(std::string("should manage multiple user queries"), [=]() mutable
            {
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Response to User Queries");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledTimes(3);
                expect(mockScenarioService["waitForCompletion"])->toHaveBeenCalledTimes(3);
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should throw error when missing scenario service"), [=]() mutable
            {
                auto brokenRuntime = utils::assign(object{
                    , 
                    object::pair{std::string("getService"), vi->fn()->mockReturnValue(undefined)}
                }, mockRuntime);
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Onboarding Process");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std::string("Scenario service not found")); });
            }
            );
            it(std::string("should handle operation timeouts"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<SocialMediaManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Onboarding Process");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std::string("Agent did not complete onboarding response in time")); });
            }
            );
        }
        );
    }
    );
}

MAIN
