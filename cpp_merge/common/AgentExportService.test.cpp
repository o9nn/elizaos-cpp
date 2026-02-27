#include "AgentExportService.test.h"

void Main(void)
{
    describe(std:("AgentExportService"), [=]() mutable
    {
        shared<std::shared_ptr<UUID>> mockAgentId;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<AgentServer>> mockServerInstance;
        shared<any> mockDb;
        beforeEach([=]() mutable
        {
            mockAgentId = as<std::shared_ptr<UUID>>(std:("test-agent-id"));
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
            mockDb = object{
                object::pair{std:("select"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std:("from"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std:("where"), [=]() mutable
                {
                    return Promise->resolve(array<any>());
                }
                }, 
                object::pair{std:("execute"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }
            };
            mockServerInstance = as<any>(object{
                object::pair{std:("db"), mockDb}
            });
        }
        );
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance of AgentExportService"), [=]() mutable
            {
                auto exportService = std::make_shared<AgentExportService>(mockAgentId, mockRuntime, mockServerInstance);
                expect(exportService)->toBeDefined();
                expect(exportService)->toBeInstanceOf(AgentExportService);
            }
            );
        }
        );
        describe(std:("exportToZip"), [=]() mutable
        {
            it(std:("should throw error if agent not found"), [=]() mutable
            {
                mockDb["where"] = [=]() mutable
                {
                    return Promise->resolve(array<any>());
                };
                auto exportService = std::make_shared<AgentExportService>(mockAgentId, mockRuntime, mockServerInstance);
                std::async([=]() { expect(exportService->exportToZip())->rejects->toThrow(std:("Agent test-agent-id not found")); });
            }
            );
        }
        );
        describe(std:("cleanup"), [=]() mutable
        {
            it(std:("should handle cleanup when pool is not initialized"), [=]() mutable
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
