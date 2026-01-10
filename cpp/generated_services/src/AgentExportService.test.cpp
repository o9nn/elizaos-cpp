#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/services/AgentExportService.test.h"

void Main(void)
{
    describe(std::string("AgentExportService"), [=]() mutable
    {
        shared<std::shared_ptr<UUID>> mockAgentId;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<AgentServer>> mockServerInstance;
        shared<any> mockDb;
        beforeEach([=]() mutable
        {
            mockAgentId = as<std::shared_ptr<UUID>>(std::string("test-agent-id"));
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
            mockDb = object{
                object::pair{std::string("select"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std::string("from"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std::string("where"), [=]() mutable
                {
                    return Promise->resolve(array<any>());
                }
                }, 
                object::pair{std::string("execute"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }
            };
            mockServerInstance = as<any>(object{
                object::pair{std::string("db"), mockDb}
            });
        }
        );
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create an instance of AgentExportService"), [=]() mutable
            {
                auto exportService = std::make_shared<AgentExportService>(mockAgentId, mockRuntime, mockServerInstance);
                expect(exportService)->toBeDefined();
                expect(exportService)->toBeInstanceOf(AgentExportService);
            }
            );
        }
        );
        describe(std::string("exportToZip"), [=]() mutable
        {
            it(std::string("should throw error if agent not found"), [=]() mutable
            {
                mockDb["where"] = [=]() mutable
                {
                    return Promise->resolve(array<any>());
                };
                auto exportService = std::make_shared<AgentExportService>(mockAgentId, mockRuntime, mockServerInstance);
                std::async([=]() { expect(exportService->exportToZip())->rejects->toThrow(std::string("Agent test-agent-id not found")); });
            }
            );
        }
        );
        describe(std::string("cleanup"), [=]() mutable
        {
            it(std::string("should handle cleanup when pool is not initialized"), [=]() mutable
            {
                auto exportService = std::make_shared<AgentExportService>(mockAgentId, mockRuntime, mockServerInstance);
                std::async([=]() { expect(exportService->cleanup())->resolves->toBeUndefined(); });
            }
            );
        }
        );
    }
    );
}

MAIN
