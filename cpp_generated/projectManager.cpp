#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/projectManager.test.h"

void Main(void)
{
    describe(std::string("ProjectManagerTestSuite"), [=]() mutable
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
            it(std::string("should handle project creation"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Project Creation");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Project Creation" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std::string("Project Setup Test"), std::string("Test Client"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std::string("world-id"), std::string("room-id"), std::string("Create a new project for CRM implementation"));
            }
            );
            it(std::string("should manage task assignments"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Task Assignment");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std::string("world-id"), std::string("task-management"));
            }
            );
        }
        );
        describe(std::string("Reporting & Communication"), [=]() mutable
        {
            it(std::string("should generate status reports"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Status Reporting");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std::string("world-id"), std::string("room-id"), std::string("Generate weekly progress report for executive review"));
            }
            );
            it(std::string("should schedule meetings"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Meeting Scheduling");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std::string("world-id"), std::string("meetings"));
            }
            );
        }
        );
        describe(std::string("Configuration Validation"), [=]() mutable
        {
            it(std::string("should have required project management plugins"), [=]() mutable
            {
                expect(projectManager->character->plugins)->toEqual(expect->arrayContaining(array<string>{ std::string("@elizaos/plugin-sql"), std::string("@elizaos/plugin-discord"), std::string("@elizaos/plugin-pdf") }));
            }
            );
            it(std::string("should maintain professional communication style"), [=]() mutable
            {
                expect(projectManager->character->style->all)->toContain(std::string("Use clear, concise, and professional language"));
                expect(projectManager->character->style->chat)->toContain(std::string("Don't be annoying or verbose"));
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
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Project Creation");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std::string("Scenario service not found")); });
            }
            );
            it(std::string("should handle report generation timeouts"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Status Reporting");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std::string("Status report generation timed out")); });
            }
            );
        }
        );
    }
    );
}

MAIN
