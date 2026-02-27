#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/projectManager.test.h"

void Main(void)
{
    describe(std:("ProjectManagerTestSuite"), [=]() mutable
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
            it(std:("should handle project creation"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Project Creation");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Project Creation" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std:("Project Setup Test"), std:("Test Client"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std:("world-id"), std:("room-id"), std:("Create a new project for CRM implementation"));
            }
            );
            it(std:("should manage task assignments"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Task Assignment");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std:("world-id"), std:("task-management"));
            }
            );
        }
        );
        describe(std:("Reporting & Communication"), [=]() mutable
        {
            it(std:("should generate status reports"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Status Reporting");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(expect->anything(), std:("world-id"), std:("room-id"), std:("Generate weekly progress report for executive review"));
            }
            );
            it(std:("should schedule meetings"), [=]() mutable
            {
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Meeting Scheduling");
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createRoom"])->toHaveBeenCalledWith(std:("world-id"), std:("meetings"));
            }
            );
        }
        );
        describe(std:("Configuration Validation"), [=]() mutable
        {
            it(std:("should have required project management plugins"), [=]() mutable
            {
                expect(projectManager->character->plugins)->toEqual(expect->arrayContaining(array<string>{ std:("@elizaos/plugin-sql"), std:("@elizaos/plugin-discord"), std:("@elizaos/plugin-pdf") }));
            }
            );
            it(std:("should maintain professional communication style"), [=]() mutable
            {
                expect(projectManager->character->style->all)->toContain(std:("Use clear, concise, and professional language"));
                expect(projectManager->character->style->chat)->toContain(std:("Don't be annoying or verbose"));
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
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Project Creation");
                }
                );
                std::async([=]() { expect(test["fn"](brokenRuntime))->rejects->toThrow(std:("Scenario service not found")); });
            }
            );
            it(std:("should handle report generation timeouts"), [=]() mutable
            {
                mockScenarioService["waitForCompletion"]["mockResolvedValue"](false);
                auto testSuite = std::make_shared<ProjectManagerTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Status Reporting");
                }
                );
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std:("Status report generation timed out")); });
            }
            );
        }
        );
    }
    );
}

MAIN
