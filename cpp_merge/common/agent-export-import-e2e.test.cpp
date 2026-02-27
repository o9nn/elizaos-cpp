#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/agent-export-import-e2e.test.h"

void Main(void)
{
    describe(std:("Agent Export/Import E2E Test"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<string> testDbPath;
        shared<std::shared_ptr<IAgentRuntime>> sourceAgent;
        shared<std::shared_ptr<UUID>> targetAgentId;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std:("test-db-export-import-") + Date->now() + string_empty);
            process->env->PGLITE_DATA_DIR = testDbPath;
            agentServer = std::make_shared<AgentServer>();
            auto dbUrl = std:("pglite://") + testDbPath + string_empty;
            std::async([=]() { agentServer->initialize(dbUrl); });
            auto character = object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-char-1"))}, 
                object::pair{std:("name"), std:("Test Export Agent")}, 
                object::pair{std:("bio"), array<string>{ std:("An agent for testing export/import") }}, 
                object::pair{std:("topics"), array<string>{ std:("testing"), std:("export"), std:("import") }}, 
                object::pair{std:("clients"), array<any>()}, 
                object::pair{std:("plugins"), array<any>()}, 
                object::pair{std:("settings"), object{
                    object::pair{std:("model"), std:("gpt-4")}, 
                    object::pair{std:("secrets"), object{}}
                }}, 
                object::pair{std:("modelProvider"), std:("openai")}
            };
            auto db = createDatabaseAdapter(object{
                object::pair{std:("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std:("test-agent-export-1")));
            std::async([=]() { db->init(); });
            sourceAgent = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-export-1"))}, 
                object::pair{std:("character"), std:("character")}, 
                object::pair{std:("adapter"), db}, 
                object::pair{std:("token"), OR((process->env->OPENAI_API_KEY), (std:("test-token")))}, 
                object::pair{std:("serverUrl"), std:("http://localhost:7777")}
            }));
            std::async([=]() { agentServer->registerAgent(sourceAgent); });
            std::async([=]() { db->createMemory(object{
                object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-memory-1"))}, 
                object::pair{std:("agentId"), sourceAgent->agentId}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("This is a test memory for export")}
                }}, 
                object::pair{std:("roomId"), as<std::shared_ptr<UUID>>(std:("test-room-1"))}, 
                object::pair{std:("entityId"), as<std::shared_ptr<UUID>>(std:("test-entity-1"))}, 
                object::pair{std:("unique"), true}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("type"), std:("test")}
                }}
            }); });
            targetAgentId = as<std::shared_ptr<UUID>>(std:("test-agent-import-1"));
        }
        );
        afterAll([=]() mutable
        {
            if (agentServer->server) {
                std::async([=]() { std::make_shared<Promise<void>>([=](auto resolve) mutable
                {
                    agentServer->server->close([=]() mutable
                    {
                        return resolve();
                    }
                    );
                }
                ); });
            }
            if (fs->existsSync(testDbPath)) {
                fs->rmSync(testDbPath, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
        }
        );
        it(std:("should and import agent data successfully"), [=]() mutable
        {
            auto exportService = std::make_shared<AgentExportService>(sourceAgent->agentId, sourceAgent, agentServer);
            shared zipStream = std::async([=]() { exportService->exportToZip(); });
            shared chunks = array<std::shared_ptr<Buffer>>();
            std::async([=]() { std::make_shared<Promise<void>>([=](auto resolve, auto reject) mutable
            {
                zipStream->on(std:("data"), [=](auto chunk) mutable
                {
                    return chunks->push(chunk);
                }
                );
                zipStream->on(std:("end"), [=]() mutable
                {
                    return resolve();
                }
                );
                zipStream->on(std:("error"), reject);
            }
            ); });
            auto zipBuffer = Buffer::concat(chunks);
            std::async([=]() { exportService->cleanup(); });
            auto zip = std::make_shared<AdmZip>(zipBuffer);
            auto entries = zip->getEntries();
            auto entryNames = entries->map([=](auto e) mutable
            {
                return e["entryName"];
            }
            );
            expect(entryNames)->toContain(std:("manifest.json"));
            expect(entryNames)->toContain(std:("database/agent.json"));
            expect(entryNames)->toContain(std:("database/memories.json"));
            auto importService = std::make_shared<AgentImportService>(targetAgentId, agentServer);
            auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                object::pair{std:("overwrite"), true}, 
                object::pair{std:("validateManifest"), true}
            }); });
            std::async([=]() { importService->cleanup(); });
            expect(result->success)->toBe(true);
            expect(result->agentId)->toBe(targetAgentId);
            expect(result->agentName)->toBe(std:("Test Export Agent"));
            expect(result->tablesImported)->toContain(std:("agent"));
            expect(result->tablesImported)->toContain(std:("memories"));
            auto targetDb = createDatabaseAdapter(object{
                object::pair{std:("dataDir"), testDbPath}
            }, targetAgentId);
            std::async([=]() { targetDb->init(); });
            auto memories = std::async([=]() { targetDb->getMemories(object{
                object::pair{std:("agentId"), targetAgentId}, 
                object::pair{std:("unique"), true}, 
                object::pair{std:("count"), 10}
            }); });
            expect(memories->length)->toBe(1);
            expect(const_(memories)[0]->content->text)->toBe(std:("This is a test memory for export"));
        }
        );
        it(std:("should handle of non-existent agent"), [=]() mutable
        {
            auto nonExistentId = as<std::shared_ptr<UUID>>(std:("non-existent-agent"));
            auto exportService = std::make_shared<AgentExportService>(nonExistentId, as<std::shared_ptr<IAgentRuntime>>(object{}), agentServer);
            std::async([=]() { expect(exportService->exportToZip())->rejects->toThrow(std:("Agent non-existent-agent not found")); });
            std::async([=]() { exportService->cleanup(); });
        }
        );
        it(std:("should validate import manifest"), [=]() mutable
        {
            auto invalidZip = std::make_shared<AdmZip>();
            invalidZip->addFile(std:("test.txt"), Buffer::from(std:("invalid content")));
            auto importService = std::make_shared<AgentImportService>(targetAgentId, agentServer);
            std::async([=]() { expect(importService->importFromZip(invalidZip->toBuffer()))->rejects->toThrow(std:("Invalid export: manifest.json not found")); });
            std::async([=]() { importService->cleanup(); });
        }
        );
    }
    );
}

MAIN
