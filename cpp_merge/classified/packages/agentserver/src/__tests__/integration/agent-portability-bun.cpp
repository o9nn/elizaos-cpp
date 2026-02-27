#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/agent-portability-bun.test.h"

void Main(void)
{
    describe(std:("Agent Portability Integration"), [=]() mutable
    {
        shared testAgentId = as<std::shared_ptr<UUID>>(std:("test-agent-123"));
        shared createMockServer = [=](auto agentExists = true) mutable
        {
            shared mockDb = object{
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
                    return Promise->resolve((agentExists) ? array<object>{ object{
                        object::pair{std:("id"), testAgentId}, 
                        object::pair{std:("name"), std:("Test Agent")}
                    } } : array<any>());
                }
                }, 
                object::pair{std:("insert"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std:("values"), [=]() mutable
                {
                    return Promise->resolve();
                }
                }, 
                object::pair{std:("delete"), [=]() mutable
                {
                    return mockDb;
                }
                }, 
                object::pair{std:("transaction"), [=](auto fn) mutable
                {
                    shared tx = object{
                        object::pair{std:("select"), [=]() mutable
                        {
                            return tx;
                        }
                        }, 
                        object::pair{std:("from"), [=]() mutable
                        {
                            return tx;
                        }
                        }, 
                        object::pair{std:("where"), [=]() mutable
                        {
                            return Promise->resolve(array<any>());
                        }
                        }, 
                        object::pair{std:("insert"), [=]() mutable
                        {
                            return tx;
                        }
                        }, 
                        object::pair{std:("values"), [=]() mutable
                        {
                            return Promise->resolve();
                        }
                        }, 
                        object::pair{std:("delete"), [=]() mutable
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
                object::pair{std:("db"), mockDb}
            });
        };
        describe(std:("Export Service"), [=]() mutable
        {
            it(std:("should create a valid ZIP structure"), [=]() mutable
            {
                auto mockServer = createMockServer(true);
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
                shared queryCount = 0;
                mockServer["db"]["where"] = [=]() mutable
                {
                    queryCount++;
                    auto responses = array<array<object>>{ array<object>{ object{
                        object::pair{std:("id"), testAgentId}, 
                        object::pair{std:("name"), std:("Test Agent")}, 
                        object::pair{std:("enabled"), true}
                    } }, array<object>{ object{
                        object::pair{std:("id"), std:("entity-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("names"), array<string>{ std:("Entity") }}
                    } }, array<any>(), array<any>(), array<any>(), array<object>{ object{
                        object::pair{std:("id"), std:("mem-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Test")}
                        }}, 
                        object::pair{std:("embedding"), array<double>{ 0.1, 0.2 }}
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
                    auto zip = std::make_shared<AdmZip>(zipBuffer);
                    auto entries = zip->getEntries();
                    auto entryNames = entries->map([=](auto e) mutable
                    {
                        return e["entryName"];
                    }
                    );
                    expect(entryNames)->toContain(std:("manifest.json"));
                    expect(entryNames)->toContain(std:("database/agent.json"));
                    expect(entryNames)->toContain(std:("database/entities.json"));
                    expect(entryNames)->toContain(std:("database/memories.json"));
                    auto manifestEntry = zip->getEntry(std:("manifest.json"));
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
            it(std:("should handle non-existent agent gracefully"), [=]() mutable
            {
                auto mockServer = createMockServer(false);
                auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{});
                auto exportService = std::make_shared<AgentExportService>(testAgentId, mockRuntime, mockServer);
                std::async([=]() { expect(exportService->exportToZip())->rejects->toThrow(std:("Agent ") + testAgentId + std:(" not found")); });
                std::async([=]() { exportService->cleanup(); });
            }
            );
        }
        );
        describe(std:("Import Service"), [=]() mutable
        {
            it(std:("should validate ZIP structure before import"), [=]() mutable
            {
                auto mockServer = createMockServer();
                auto invalidZip = std::make_shared<AdmZip>();
                invalidZip->addFile(std:("test.txt"), Buffer::from(std:("test")));
                auto importService = std::make_shared<AgentImportService>(testAgentId, mockServer);
                std::async([=]() { expect(importService->importFromZip(invalidZip->toBuffer()))->rejects->toThrow(std:("Invalid export: manifest.json not found")); });
                std::async([=]() { importService->cleanup(); });
            }
            );
            it(std:("should validate manifest version"), [=]() mutable
            {
                auto mockServer = createMockServer();
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("99.0.0")}, 
                    object::pair{std:("agentId"), std:("source-agent")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent") }}, 
                    object::pair{std:("fileCount"), 0}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                zip->addFile(std:("database/agent.json"), Buffer::from(std:("[]")));
                auto importService = std::make_shared<AgentImportService>(testAgentId, mockServer);
                std::async([=]() { expect(importService->importFromZip(zip->toBuffer()))->rejects->toThrow(std:("Unsupported version: 99.0.0")); });
                std::async([=]() { importService->cleanup(); });
            }
            );
            it(std:("should successfully import valid ZIP"), [=]() mutable
            {
                auto mockServer = createMockServer();
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("agentId"), std:("source-agent")}, 
                    object::pair{std:("agentName"), std:("Test Agent")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 0}, 
                    object::pair{std:("elizaVersion"), std:("1.0.0")}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                auto agentData = array<object>{ object{
                    object::pair{std:("id"), std:("source-agent")}, 
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("enabled"), true}, 
                    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
                } };
                zip->addFile(std:("database/agent.json"), Buffer::from(JSON->stringify(agentData)));
                auto emptyTables = array<string>{ std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") };
                for (auto& table : emptyTables)
                {
                    zip->addFile(std:("database/") + table + std:(".json"), Buffer::from(std:("[]")));
                }
                auto importService = std::make_shared<AgentImportService>(testAgentId, mockServer);
                auto result = std::async([=]() { importService->importFromZip(zip->toBuffer(), object{
                    object::pair{std:("overwrite"), true}, 
                    object::pair{std:("validateManifest"), true}
                }); });
                expect(result->success)->toBe(true);
                expect(result->agentId)->toBe(testAgentId);
                expect(result->agentName)->toBe(std:("Test Agent"));
                expect(result->tablesImported->get_length())->toBe(9);
                std::async([=]() { importService->cleanup(); });
            }
            );
        }
        );
        describe(std:("Export/Import Cycle"), [=]() mutable
        {
            it(std:("should maintain data integrity through export/import cycle"), [=]() mutable
            {
                auto exportZip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("agentId"), std:("cycle-test-agent")}, 
                    object::pair{std:("agentName"), std:("Cycle Test Agent")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 1}, 
                    object::pair{std:("elizaVersion"), std:("1.0.0")}
                };
                exportZip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                auto agentData = array<object>{ object{
                    object::pair{std:("id"), std:("cycle-test-agent")}, 
                    object::pair{std:("name"), std:("Cycle Test Agent")}, 
                    object::pair{std:("enabled"), true}, 
                    object::pair{std:("bio"), array<string>{ std:("Test bio line 1"), std:("Test bio line 2") }}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("theme"), std:("dark")}, 
                        object::pair{std:("language"), std:("en")}
                    }}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("version"), 1}, 
                        object::pair{std:("tags"), array<string>{ std:("test"), std:("cycle") }}
                    }}, 
                    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
                } };
                exportZip->addFile(std:("database/agent.json"), Buffer::from(JSON->stringify(agentData, nullptr, 2)));
                auto memories = array<object>{ object{
                    object::pair{std:("id"), std:("memory-1")}, 
                    object::pair{std:("agentId"), std:("cycle-test-agent")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Test memory with embedding")}
                    }}, 
                    object::pair{std:("embedding"), ((array(768)))->fill(0.1)}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("type"), std:("test")}
                    }}, 
                    object::pair{std:("createdAt"), Date->now()}
                } };
                exportZip->addFile(std:("database/memories.json"), Buffer::from(JSON->stringify(memories, nullptr, 2)));
                auto emptyTables = array<string>{ std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("relationships"), std:("tasks"), std:("server_agents") };
                for (auto& table : emptyTables)
                {
                    exportZip->addFile(std:("database/") + table + std:(".json"), Buffer::from(std:("[]")));
                }
                exportZip->addFile(std:("uploads/test.txt"), Buffer::from(std:("test file content")));
                auto zipBuffer = exportZip->toBuffer();
                auto verifyZip = std::make_shared<AdmZip>(zipBuffer);
                auto entries = verifyZip->getEntries();
                expect(entries->length)->toBe(11);
                auto manifestEntry = verifyZip->getEntry(std:("manifest.json"));
                auto readManifest = JSON->parse(verifyZip->readAsText(manifestEntry));
                expect(readManifest["agentId"])->toBe(std:("cycle-test-agent"));
                auto agentEntry = verifyZip->getEntry(std:("database/agent.json"));
                auto readAgent = JSON->parse(verifyZip->readAsText(agentEntry));
                expect(const_(readAgent)[0]["bio"])->toEqual(array<string>{ std:("Test bio line 1"), std:("Test bio line 2") });
                expect(const_(readAgent)[0]["settings"]["theme"])->toBe(std:("dark"));
                auto memoriesEntry = verifyZip->getEntry(std:("database/memories.json"));
                auto readMemories = JSON->parse(verifyZip->readAsText(memoriesEntry));
                expect(const_(readMemories)[0]["embedding"]["length"])->toBe(768);
                expect(const_(readMemories)[0]["content"]["text"])->toBe(std:("Test memory with embedding"));
                auto fileEntry = verifyZip->getEntry(std:("uploads/test.txt"));
                expect(fileEntry->getData()->toString())->toBe(std:("test file content"));
            }
            );
        }
        );
    }
    );
}

MAIN
