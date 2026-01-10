#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/agent-export-import-e2e.test.h"

void Main(void)
{
    describe(std::string("Agent Export/Import E2E Test"), [=]() mutable
    {
        shared<std::shared_ptr<AgentServer>> agentServer;
        shared<string> testDbPath;
        shared<std::shared_ptr<IAgentRuntime>> sourceAgent;
        shared<std::shared_ptr<UUID>> targetAgentId;
        beforeAll([=]() mutable
        {
            testDbPath = path->join(__dirname, std::string("test-db-export-import-") + Date->now() + string_empty);
            process->env->PGLITE_DATA_DIR = testDbPath;
            agentServer = std::make_shared<AgentServer>();
            auto dbUrl = std::string("pglite://") + testDbPath + string_empty;
            std::async([=]() { agentServer->initialize(dbUrl); });
            auto character = object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-char-1"))}, 
                object::pair{std::string("name"), std::string("Test Export Agent")}, 
                object::pair{std::string("bio"), array<string>{ std::string("An agent for testing export/import") }}, 
                object::pair{std::string("topics"), array<string>{ std::string("testing"), std::string("export"), std::string("import") }}, 
                object::pair{std::string("clients"), array<any>()}, 
                object::pair{std::string("plugins"), array<any>()}, 
                object::pair{std::string("settings"), object{
                    object::pair{std::string("model"), std::string("gpt-4")}, 
                    object::pair{std::string("secrets"), object{}}
                }}, 
                object::pair{std::string("modelProvider"), std::string("openai")}
            };
            auto db = createDatabaseAdapter(object{
                object::pair{std::string("dataDir"), testDbPath}
            }, as<std::shared_ptr<UUID>>(std::string("test-agent-export-1")));
            std::async([=]() { db->init(); });
            sourceAgent = std::make_shared<AgentRuntime>(as<any>(object{
                object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-export-1"))}, 
                object::pair{std::string("character"), std::string("character")}, 
                object::pair{std::string("adapter"), db}, 
                object::pair{std::string("token"), OR((process->env->OPENAI_API_KEY), (std::string("test-token")))}, 
                object::pair{std::string("serverUrl"), std::string("http://localhost:7777")}
            }));
            std::async([=]() { agentServer->registerAgent(sourceAgent); });
            std::async([=]() { db->createMemory(object{
                object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-memory-1"))}, 
                object::pair{std::string("agentId"), sourceAgent->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("This is a test memory for export")}
                }}, 
                object::pair{std::string("roomId"), as<std::shared_ptr<UUID>>(std::string("test-room-1"))}, 
                object::pair{std::string("entityId"), as<std::shared_ptr<UUID>>(std::string("test-entity-1"))}, 
                object::pair{std::string("unique"), true}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("type"), std::string("test")}
                }}
            }); });
            targetAgentId = as<std::shared_ptr<UUID>>(std::string("test-agent-import-1"));
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
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
        }
        );
        it(std::string("should export and import agent data successfully"), [=]() mutable
        {
            auto exportService = std::make_shared<AgentExportService>(sourceAgent->agentId, sourceAgent, agentServer);
            shared zipStream = std::async([=]() { exportService->exportToZip(); });
            shared chunks = array<std::shared_ptr<Buffer>>();
            std::async([=]() { std::make_shared<Promise<void>>([=](auto resolve, auto reject) mutable
            {
                zipStream->on(std::string("data"), [=](auto chunk) mutable
                {
                    return chunks->push(chunk);
                }
                );
                zipStream->on(std::string("end"), [=]() mutable
                {
                    return resolve();
                }
                );
                zipStream->on(std::string("error"), reject);
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
            expect(entryNames)->toContain(std::string("manifest.json"));
            expect(entryNames)->toContain(std::string("database/agent.json"));
            expect(entryNames)->toContain(std::string("database/memories.json"));
            auto importService = std::make_shared<AgentImportService>(targetAgentId, agentServer);
            auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                object::pair{std::string("overwrite"), true}, 
                object::pair{std::string("validateManifest"), true}
            }); });
            std::async([=]() { importService->cleanup(); });
            expect(result->success)->toBe(true);
            expect(result->agentId)->toBe(targetAgentId);
            expect(result->agentName)->toBe(std::string("Test Export Agent"));
            expect(result->tablesImported)->toContain(std::string("agent"));
            expect(result->tablesImported)->toContain(std::string("memories"));
            auto targetDb = createDatabaseAdapter(object{
                object::pair{std::string("dataDir"), testDbPath}
            }, targetAgentId);
            std::async([=]() { targetDb->init(); });
            auto memories = std::async([=]() { targetDb->getMemories(object{
                object::pair{std::string("agentId"), targetAgentId}, 
                object::pair{std::string("unique"), true}, 
                object::pair{std::string("count"), 10}
            }); });
            expect(memories->length)->toBe(1);
            expect(const_(memories)[0]->content->text)->toBe(std::string("This is a test memory for export"));
        }
        );
        it(std::string("should handle export of non-existent agent"), [=]() mutable
        {
            auto nonExistentId = as<std::shared_ptr<UUID>>(std::string("non-existent-agent"));
            auto exportService = std::make_shared<AgentExportService>(nonExistentId, as<std::shared_ptr<IAgentRuntime>>(object{}), agentServer);
            std::async([=]() { expect(exportService->exportToZip())->rejects->toThrow(std::string("Agent non-existent-agent not found")); });
            std::async([=]() { exportService->cleanup(); });
        }
        );
        it(std::string("should validate import manifest"), [=]() mutable
        {
            auto invalidZip = std::make_shared<AdmZip>();
            invalidZip->addFile(std::string("test.txt"), Buffer::from(std::string("invalid content")));
            auto importService = std::make_shared<AgentImportService>(targetAgentId, agentServer);
            std::async([=]() { expect(importService->importFromZip(invalidZip->toBuffer()))->rejects->toThrow(std::string("Invalid export: manifest.json not found")); });
            std::async([=]() { importService->cleanup(); });
        }
        );
    }
    );
}

MAIN
