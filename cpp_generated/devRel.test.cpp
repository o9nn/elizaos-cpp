#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/tests/devRel.test.h"

void Main(void)
{
    describe(std::string("devRel Agent Test Suite"), [=]() mutable
    {
        shared<any> mockScenarioService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            vi->restoreAllMocks();
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
        describe(std::string("Core Developer Support Functionality"), [=]() mutable
        {
            it(std::string("should handle technical documentation requests"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Documentation Query");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Documentation Query" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std::string("Doc Test"), std::string("Test Developer"));
                expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std::string("world-id"), std::string("room-id"), std::string("How do I implement custom actions in ElizaOS?"));
            }
            );
            it(std::string("should assist with plugin integration"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Plugin Integration");
                }
                );
                expect(test)->toBeDefined();
                if (!test) {
                    throw any(std::make_shared<Error>(std::string("Test "Test Plugin Integration" not found")));
                }
                std::async([=]() { test["fn"](mockRuntime); });
                expect(mockScenarioService["createWorld"])->toHaveBeenCalledWith(std::string("Plugin Test"), std::string("Test Developer"));
            }
            );
        }
        );
        describe(std::string("Knowledge Base Integration"), [=]() mutable
        {
            it(std::string("should reference documentation in responses"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Documentation Reference");
                }
                );
                mockScenarioService["sendMessage"]["mockImplementationOnce"]([=](auto _, auto __, auto ___, auto msg) mutable
                {
                    return Promise->resolve(object{
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), string_empty + msg + std::string("\
Refer to documentation: https://docs.elizaos.com")}
                        }}
                    });
                }
                );
                std::async([=]() { test["fn"](mockRuntime); });
                auto messages = mockScenarioService["sendMessage"]["mock"]["calls"];
                expect(const_(const_(messages)[0])[3])->toMatch((new RegExp(std::string("How do I implement custom actions in ElizaOS"))));
            }
            );
            it(std::string("should access source code knowledge when enabled"), [=]() mutable
            {
                auto originalEnv = process->env->DEVREL_IMPORT_KNOWLEDGE;
                process->env->DEVREL_IMPORT_KNOWLEDGE = std::string("true");
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
                            return t["name"] == std::string("Test Source Code Knowledge");
                        }
                        );
                        expect(test)->toBeDefined();
                        if (!test) {
                            throw any(std::make_shared<Error>(std::string("Test "Test Source Code Knowledge" not found")));
                        }
                        mockScenarioService["sendMessage"]["mockImplementationOnce"]([=](auto _, auto __, auto ___, auto msg) mutable
                        {
                            return Promise->resolve(object{
                                object::pair{std::string("content"), object{
                                    object::pair{std::string("text"), string_empty + msg + std::string("\
Source code location: src/elizaos/core/agent-runtime.ts")}
                                }}
                            });
                        }
                        );
                        std::async([=]() { test["fn"](mockRuntime); });
                        expect(mockScenarioService["sendMessage"])->toHaveBeenCalledWith(mockRuntime, std::string("world-id"), std::string("room-id"), expect->stringContaining(std::string("AgentRuntime")));
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
        describe(std::string("Configuration Validation"), [=]() mutable
        {
            it(std::string("should have correct developer-focused settings"), [=]() mutable
            {
                expect(devRel["character"]->settings->avatar)->toBeDefined();
                expect(devRel["character"]->plugins)->toContain(std::string("@elizaos/plugin-discord"));
            }
            );
            it(std::string("should maintain technical response style"), [=]() mutable
            {
                expect(devRel["character"]->style->all)->toContain(std::string("Use clear, concise, and technical language"));
            }
            );
        }
        );
        describe(std::string("Error Handling"), [=]() mutable
        {
            it(std::string("should handle missing documentation paths"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Missing Documentation");
                }
                );
                mockScenarioService["sendMessage"]["mockRejectedValueOnce"](std::make_shared<Error>(std::string("Documentation not found")));
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std::string("Documentation not found")); });
            }
            );
            it(std::string("should handle codebase navigation errors"), [=]() mutable
            {
                auto testSuite = std::make_shared<DevRelTestSuite>();
                auto test = testSuite->tests->find([=](auto t) mutable
                {
                    return t["name"] == std::string("Test Code Navigation");
                }
                );
                mockScenarioService["waitForCompletion"]["mockRejectedValueOnce"](std::make_shared<Error>(std::string("Could not locate code reference")));
                std::async([=]() { expect(test["fn"](mockRuntime))->rejects->toThrow(std::string("Could not locate code reference")); });
            }
            );
        }
        );
    }
    );
}

MAIN
