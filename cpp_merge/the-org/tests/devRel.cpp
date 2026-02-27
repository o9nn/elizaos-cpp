#include "devRel.test.h"

void Main(void)
{
    describe(std:("devRel Agent Test Suite"), [=]() mutable
    {
        shared<any> mockScenarioService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            vi->restoreAllMocks();
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
        describe(std:("Core Developer Support Functionality"), [=]() mutable
        {
            it(std:("should handle technical documentation requests"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Documentation Query");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Documentation Query" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std:("Doc Test"), std:("Test Developer"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std:("world-id"), std:("room-id"), std:("How do I implement custom actions in ElizaOS?"));
            }
            );
            it(std:("should assist with plugin integration"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Plugin Integration");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std:("Test "Test Plugin Integration" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std:("Plugin Test"), std:("Test Developer"));
            }
            );
        }
        );
        describe(std:("Knowledge Base Integration"), [=]() mutable
        {
            it(std:("should reference documentation in responses"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Documentation Reference");
                }
                );
                mockScenarioService["sendMessage"]["mockImplementationOnce"]([=](auto _, auto __, auto ___, auto msg) mutable
                {
                    return Promise->resolve(object{
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), string_empty + msg + std:("\
Refer to documentation: https://docs.elizaos.com")}
                        }}
                    });
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                auto messages = mockScenarioService["sendMessage"]["mock"]["calls"];
                expect(const_(const_(messages)[0])[3])->toMatch((new RegExp(std:("How do I implement custom actions in ElizaOS"))));
            }
            );
            it(std:("should access source code knowledge when enabled"), [=]() mutable
            {
                auto originalEnv = process->env->DEVREL_IMPORT_KNOWLEDGE;
                process->env->DEVREL_IMPORT_KNOWLEDGE = std:("true");
                {
                    utils::finally __finally4046_4288([&]() mutable
                    {
                        if (originalEnv != undefined) {
                            process->env->DEVREL_IMPORT_KNOWLEDGE = originalEnv;
                        } else {
                            process->env.Delete("DEVREL_IMPORT_KNOWLEDGE");
                        }
                    });
                    try
                    {
                        auto testSuite = std::make_shared<DevRelTestSuite>();
                        auto test = testSuite->tests->find([=](auto t) mutable
                        {
                            return t["name"] == std:("Test Source Code Knowledge");
                        }
                        );
                        expect(test)->toBeDefined();
                        if (!test) {
                            throw any(std::make_shared<Error>(std:("Test "Test Source Code Knowledge" not found")));
                        }
                        mockScenarioService["sendMessage"]["mockImplementationOnce"]([=](auto _, auto __, auto ___, auto msg) mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std:("content"), object{
                                    object::pair{std:("text"), string_empty + msg + std:("\
Source code location: src/elizaos/core/agent-runtime.ts")}
                                }}
                            });
                        }
                        );
                        std::async([=]() { test["fn"](mockRuntime); });
                        expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std:("world-id"), std:("room-id"), expect->stringContaining(std:("AgentRuntime")));
                    }
                    catch (...)
                    {
                        throw;
                    }
                }
            }
            );
        }
        );
        describe(std:("Configuration Validation"), [=]() mutable
        {
            it(std:("should have correct developer-focused settings"), [=]() mutable
            {
                expect(devRel["character"]->settings->avatar)->toBeDefined();
                expect(devRel["character"]->plugins)->toContain(std:("@elizaos/plugin-discord"));
            }
            );
            it(std:("should maintain technical response style"), [=]() mutable
            {
                expect(devRel["character"]->style->all)->toContain(std:("Use clear, concise, and technical language"));
            }
            );
        }
        );
        describe(std:("Error Handling"), [=]() mutable
        {
            it(std:("should handle missing documentation paths"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Missing Documentation");
                }
                );
                mockScenarioService["sendMessage"]["mockRejectedValueOnce"](std::make_shared<Error>(std:("Documentation not found")));
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std:("Documentation not found")); });
            }
            );
            it(std:("should handle codebase navigation errors"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std:("Test Code Navigation");
                }
                );
                mockScenarioService["waitForCompletion"]["mockRejectedValueOnce"](std::make_shared<Error>(std:("Could not locate code reference")));
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std:("Could not locate code reference")); });
            }
            );
        }
        );
    }
    );
}

MAIN
