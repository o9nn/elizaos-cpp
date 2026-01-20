#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/agent-portability-bun.test.h"

void Main(void)
{
    describe(std::string("Agent Portability Integration"), [=]() mutable
    {
        shared testAgentId = as<std::shared_ptr<UUID>>(std::string("test-agent-123"));
        shared createMockServer = [=](auto agentExists = true) mutable
        {
            shared mockDb = object{
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
                    return Promise->resolve((agentExists) ? array<object>{ object{
                        object::pair{std::string("id"), testAgentId}, 
                        object::pair{std::string("name"), std::string("Test Agent")}
                    } } : array<any>());
                }
                }, 
                object::pair{std::string("insert"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std::string("values"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }, 
                object::pair{std::string("delete"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std::string("transaction"), [=](auto fn) mutable
                {
                    shared tx = object{
                        object::pair{std::string("select"), [=]() mutable
                        {
                            return tx;
                        }
                        }, 
                        object::pair{std::string("from"), [=]() mutable
                        {
                            return tx;
                        }
                        }, 
                        object::pair{std::string("where"), [=]() mutable
                        {
                            return Promise->resolve(array<any>());
                        }
                        }, 
                        object::pair{std::string("insert"), [=]() mutable
                        {
                            return tx;
                        }
                        }, 
                        object::pair{std::string("values"), [=]() mutable
                        {
                            return Promise->resolve();
                        }
                        }, 
                        object::pair{std::string("delete"), [=]() mutable
                        {
                            return tx;
                        }
                        }
                    };
                    return std::async([=]() { fn(tx); });
                }
                }
            };
            return as<any>(object{
                object::pair{std::string("db"), mockDb}
            });
        };
        describe(std::string("Export Service"), [=]() mutable
        {
            it(std::string("should create a valid export ZIP structure"), [=]() mutable
            {
                auto mockServer = createMockServer(true);
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
                shared queryCount = 0;
                mockServer["db"]["where"] = [=]() mutable
                {
                    queryCount++;
                    auto responses = array<array<object>>{ array<object>{ object{
                        object::pair{std::string("id"), testAgentId}, 
                        object::pair{std::string("name"), std::string("Test Agent")}, 
                        object::pair{std::string("enabled"), true}
                    } }, array<object>{ object{
                        object::pair{std::string("id"), std::string("entity-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("names"), array<string>{ std::string("Entity") }}
                    } }, array<any>(), array<any>(), array<any>(), array<object>{ object{
                        object::pair{std::string("id"), std::string("mem-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Test")}
                        }}, 
                        object::pair{std::string("embedding"), array<double>{ 0.1, 0.2 }}
                    } }, array<any>(), array<any>(), array<any>() };
                    return Promise->resolve(OR((const_(responses)[queryCount - 1]), (array<any>())));
                };
                auto exportService = std::make_shared<AgentExportService>(testAgentId, mockRuntime, mockServer);
                try
                {
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
                    auto zip = std::make_shared<AdmZip>(zipBuffer);
                    auto entries = zip->getEntries();
                    auto entryNames = entries->map([=](auto e) mutable
                    {
                        return e["entryName"];
                    }
                    );
                    expect(entryNames)->toContain(std::string("manifest.json"));
                    expect(entryNames)->toContain(std::string("database/agent.json"));
                    expect(entryNames)->toContain(std::string("database/entities.json"));
                    expect(entryNames)->toContain(std::string("database/memories.json"));
                    auto manifestEntry = zip->getEntry(std::string("manifest.json"));
                    auto manifest = JSON->parse(zip->readAsText(manifestEntry));
                    expect(manifest["agentId"])->toBe(testAgentId);
                    expect(manifest["tables"]["length"])->toBe(9);
                    std::async([=]() { exportService->cleanup(); });
                }
                catch (const any& error)
                {
                    std::async([=]() { exportService->cleanup(); });
                    throw any(error);
                }
            }
            );
            it(std::string("should handle non-existent agent gracefully"), [=]() mutable
            {
                auto mockServer = createMockServer(false);
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
                auto exportService = std::make_shared<AgentExportService>(testAgentId, mockRuntime, mockServer);
                std::async([=]() { expect(exportService->exportToZip())->rejects->toThrow(std::string("Agent ") + testAgentId + std::string(" not found")); });
                std::async([=]() { exportService->cleanup(); });
            }
            );
        }
        );
        describe(std::string("Import Service"), [=]() mutable
        {
            it(std::string("should validate ZIP structure before import"), [=]() mutable
            {
                auto mockServer = createMockServer();
                auto invalidZip = std::make_shared<AdmZip>();
                invalidZip->addFile(std::string("test.txt"), Buffer::from(std::string("test")));
                auto importService = std::make_shared<AgentImportService>(testAgentId, mockServer);
                std::async([=]() { expect(importService->importFromZip(invalidZip->toBuffer()))->rejects->toThrow(std::string("Invalid export: manifest.json not found")); });
                std::async([=]() { importService->cleanup(); });
            }
            );
            it(std::string("should validate manifest version"), [=]() mutable
            {
                auto mockServer = createMockServer();
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("99.0.0")}, 
                    object::pair{std::string("agentId"), std::string("source-agent")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent") }}, 
                    object::pair{std::string("fileCount"), 0}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                zip->addFile(std::string("database/agent.json"), Buffer::from(std::string("[]")));
                auto importService = std::make_shared<AgentImportService>(testAgentId, mockServer);
                std::async([=]() { expect(importService->importFromZip(zip->toBuffer()))->rejects->toThrow(std::string("Unsupported export version: 99.0.0")); });
                std::async([=]() { importService->cleanup(); });
            }
            );
            it(std::string("should successfully import valid ZIP"), [=]() mutable
            {
                auto mockServer = createMockServer();
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("agentId"), std::string("source-agent")}, 
                    object::pair{std::string("agentName"), std::string("Test Agent")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 0}, 
                    object::pair{std::string("elizaVersion"), std::string("1.0.0")}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                auto agentData = array<object>{ object{
                    object::pair{std::string("id"), std::string("source-agent")}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("enabled"), true}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
                } };
                zip->addFile(std::string("database/agent.json"), Buffer::from(JSON->stringify(agentData)));
                auto emptyTables = array<string>{ std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") };
                for (auto& table : emptyTables)
                {
                    zip->addFile(std::string("database/") + table + std::string(".json"), Buffer::from(std::string("[]")));
                }
                auto importService = std::make_shared<AgentImportService>(testAgentId, mockServer);
                auto result = std::async([=]() { importService->importFromZip(zip->toBuffer(), object{
                    object::pair{std::string("overwrite"), true}, 
                    object::pair{std::string("validateManifest"), true}
                }); });
                expect(result->success)->toBe(true);
                expect(result->agentId)->toBe(testAgentId);
                expect(result->agentName)->toBe(std::string("Test Agent"));
                expect(result->tablesImported->get_length())->toBe(9);
                std::async([=]() { importService->cleanup(); });
            }
            );
        }
        );
        describe(std::string("Export/Import Cycle"), [=]() mutable
        {
            it(std::string("should maintain data integrity through export/import cycle"), [=]() mutable
            {
                auto exportZip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("agentId"), std::string("cycle-test-agent")}, 
                    object::pair{std::string("agentName"), std::string("Cycle Test Agent")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 1}, 
                    object::pair{std::string("elizaVersion"), std::string("1.0.0")}
                };
                exportZip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                auto agentData = array<object>{ object{
                    object::pair{std::string("id"), std::string("cycle-test-agent")}, 
                    object::pair{std::string("name"), std::string("Cycle Test Agent")}, 
                    object::pair{std::string("enabled"), true}, 
                    object::pair{std::string("bio"), array<string>{ std::string("Test bio line 1"), std::string("Test bio line 2") }}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("theme"), std::string("dark")}, 
                        object::pair{std::string("language"), std::string("en")}
                    }}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("version"), 1}, 
                        object::pair{std::string("tags"), array<string>{ std::string("test"), std::string("cycle") }}
                    }}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
                } };
                exportZip->addFile(std::string("database/agent.json"), Buffer::from(JSON->stringify(agentData, nullptr, 2)));
                auto memories = array<object>{ object{
                    object::pair{std::string("id"), std::string("memory-1")}, 
                    object::pair{std::string("agentId"), std::string("cycle-test-agent")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Test memory with embedding")}
                    }}, 
                    object::pair{std::string("embedding"), ((array(768)))->fill(0.1)}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("type"), std::string("test")}
                    }}, 
                    object::pair{std::string("createdAt"), Date->now()}
                } };
                exportZip->addFile(std::string("database/memories.json"), Buffer::from(JSON->stringify(memories, nullptr, 2)));
                auto emptyTables = array<string>{ std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("relationships"), std::string("tasks"), std::string("server_agents") };
                for (auto& table : emptyTables)
                {
                    exportZip->addFile(std::string("database/") + table + std::string(".json"), Buffer::from(std::string("[]")));
                }
                exportZip->addFile(std::string("uploads/test.txt"), Buffer::from(std::string("test file content")));
                auto zipBuffer = exportZip->toBuffer();
                auto verifyZip = std::make_shared<AdmZip>(zipBuffer);
                auto entries = verifyZip->getEntries();
                expect(entries->length)->toBe(11);
                auto manifestEntry = verifyZip->getEntry(std::string("manifest.json"));
                auto readManifest = JSON->parse(verifyZip->readAsText(manifestEntry));
                expect(readManifest["agentId"])->toBe(std::string("cycle-test-agent"));
                auto agentEntry = verifyZip->getEntry(std::string("database/agent.json"));
                auto readAgent = JSON->parse(verifyZip->readAsText(agentEntry));
                expect(const_(readAgent)[0]["bio"])->toEqual(array<string>{ std::string("Test bio line 1"), std::string("Test bio line 2") });
                expect(const_(readAgent)[0]["settings"]["theme"])->toBe(std::string("dark"));
                auto memoriesEntry = verifyZip->getEntry(std::string("database/memories.json"));
                auto readMemories = JSON->parse(verifyZip->readAsText(memoriesEntry));
                expect(const_(readMemories)[0]["embedding"]["length"])->toBe(768);
                expect(const_(readMemories)[0]["content"]["text"])->toBe(std::string("Test memory with embedding"));
                auto fileEntry = verifyZip->getEntry(std::string("uploads/test.txt"));
                expect(fileEntry->getData()->toString())->toBe(std::string("test file content"));
            }
            );
        }
        );
    }
    );
}

MAIN
