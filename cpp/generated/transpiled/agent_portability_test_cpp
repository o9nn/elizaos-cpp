#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/agent-portability.test.h"

void Main(void)
{
    describe(std::string("Agent Portability Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<express::Application>> app;
        shared<std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>>> agents;
        shared<std::shared_ptr<AgentServer>> mockServerInstance;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared testAgentId = as<std::shared_ptr<UUID>>(std::string("test-agent-123"));
        beforeEach([=]() mutable
        {
            app = express();
            agents = std::make_shared<Map>();
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{
                object::pair{std::string("agentId"), testAgentId}, 
                object::pair{std::string("character"), object{
                    object::pair{std::string("name"), std::string("Test Agent")}
                }}
            });
            agents->set(testAgentId, mockRuntime);
            auto mockDb = object{
                object::pair{std::string("select"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("from"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("where"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("insert"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("delete"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("values"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("transaction"), vi->fn([=](auto callback) mutable
                {
                    return callback(object{
                        object::pair{std::string("insert"), vi->fn()->mockReturnThis()}, 
                        object::pair{std::string("delete"), vi->fn()->mockReturnThis()}, 
                        object::pair{std::string("values"), vi->fn()->mockReturnThis()}, 
                        object::pair{std::string("where"), vi->fn()->mockReturnThis()}
                    });
                }
                )}
            };
            mockDb["where"]->mockResolvedValue(array<object>{ object{
                object::pair{std::string("id"), testAgentId}, 
                object::pair{std::string("name"), std::string("Test Agent")}, 
                object::pair{std::string("enabled"), true}, 
                object::pair{std::string("createdAt"), std::make_shared<Date>()}, 
                object::pair{std::string("updatedAt"), std::make_shared<Date>()}
            } });
            mockServerInstance = as<any>(object{
                object::pair{std::string("db"), mockDb}, 
                object::pair{std::string("getAgents"), [=]() mutable
                {
                    return agents;
                }
                }
            });
            app->use(std::string("/api"), createApiRouter(agents, mockServerInstance));
        }
        );
        afterEach([=]() mutable
        {
            vi->clearAllMocks();
        }
        );
        describe(std::string("Export/Import Cycle"), [=]() mutable
        {
            it(std::string("should export an agent to ZIP file"), [=]() mutable
            {
                vi->spyOn(fs, std::string("existsSync"))->mockReturnValue(false);
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/export"))->expect(200)->expect(std::string("Content-Type"), std::string("application/zip")); });
                expect(const_(response->headers)[std::string("content-disposition")])->toMatch((new RegExp(std::string("attachment; filename="agent-export"))));
                expect(const_(response->headers)[std::string("x-agent-id")])->toBe(testAgentId);
            }
            );
            it(std::string("should validate ZIP file before import"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("original-id")}, 
                    object::pair{std::string("agentName"), std::string("Test Agent")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 0}, 
                    object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("elizaVersion"), std::string("1.0.0")}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                auto tables = array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") };
                for (auto& table : tables)
                {
                    zip->addFile(std::string("database/") + table + std::string(".json"), Buffer::from(std::string("[]")));
                }
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import/validate"))->attach(std::string("archive"), zipBuffer, std::string("test-export.zip"))->expect(200); });
                expect(response->body->status)->toBe(std::string("success"));
                expect(response->body->data->result->valid)->toBe(true);
                expect(response->body->data->result->manifest->agentName)->toBe(std::string("Test Agent"));
            }
            );
            it(std::string("should reject import without overwrite flag if agent exists"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                zip->addFile(std::string("manifest.json"), Buffer::from(std::string("{}")));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("false"))->attach(std::string("archive"), zipBuffer, std::string("test-export.zip"))->expect(409); });
                expect(response->body->error->code)->toBe(std::string("AGENT_EXISTS"));
                expect(response->body->error->message)->toContain(std::string("Set overwrite=true"));
            }
            );
            it(std::string("should import agent data with overwrite flag"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("original-id")}, 
                    object::pair{std::string("agentName"), std::string("Imported Agent")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 0}, 
                    object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("elizaVersion"), std::string("1.0.0")}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                auto agentData = array<object>{ object{
                    object::pair{std::string("id"), std::string("original-id")}, 
                    object::pair{std::string("name"), std::string("Imported Agent")}, 
                    object::pair{std::string("enabled"), true}, 
                    object::pair{std::string("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
                } };
                zip->addFile(std::string("database/agent.json"), Buffer::from(JSON->stringify(agentData, nullptr, 2)));
                auto emptyTables = array<string>{ std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") };
                for (auto& table : emptyTables)
                {
                    zip->addFile(std::string("database/") + table + std::string(".json"), Buffer::from(std::string("[]")));
                }
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->attach(std::string("archive"), zipBuffer, std::string("test-export.zip"))->expect(200); });
                expect(response->body->status)->toBe(std::string("success"));
                expect(response->body->data->result->success)->toBe(true);
                expect(response->body->data->result->agentName)->toBe(std::string("Imported Agent"));
                expect(response->body->data->result->tablesImported)->toHaveLength(9);
            }
            );
            it(std::string("should handle invalid ZIP file gracefully"), [=]() mutable
            {
                auto invalidZipBuffer = Buffer::from(std::string("This is not a valid ZIP file"));
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->attach(std::string("archive"), invalidZipBuffer, std::string("invalid.zip"))->expect(500); });
                expect(response->body->error->code)->toBe(std::string("IMPORT_FAILED"));
            }
            );
            it(std::string("should reject oversized ZIP files"), [=]() mutable
            {
                auto largeBuffer = Buffer::alloc(600 * 1024 * 1024);
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->attach(std::string("archive"), largeBuffer, std::string("large.zip"))->expect(413); });
                expect(response->body->error)->toBeDefined();
            }
            );
            it(std::string("should handle missing manifest.json"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                zip->addFile(std::string("database/agent.json"), Buffer::from(std::string("[]")));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->attach(std::string("archive"), zipBuffer, std::string("no-manifest.zip"))->expect(400); });
                expect(response->body->error->code)->toBe(std::string("INVALID_ARCHIVE"));
                expect(response->body->error->message)->toContain(std::string("manifest.json not found"));
            }
            );
            it(std::string("should validate manifest version"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("99.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<any>()}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->attach(std::string("archive"), zipBuffer, std::string("wrong-version.zip"))->expect(400); });
                expect(response->body->error->code)->toBe(std::string("VERSION_MISMATCH"));
                expect(response->body->error->message)->toContain(std::string("Unsupported export version"));
            }
            );
            it(std::string("should handle export errors gracefully"), [=]() mutable
            {
                agents->clear();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/export"))->expect(404); });
                expect(response->body->error->code)->toBe(std::string("AGENT_NOT_FOUND"));
            }
            );
            it(std::string("should validate agent ID format"), [=]() mutable
            {
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/invalid-uuid/export"))->expect(400); });
                expect(response->body->error->code)->toBe(std::string("INVALID_AGENT_ID"));
            }
            );
            it(std::string("should require ZIP file for import"), [=]() mutable
            {
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->expect(400); });
                expect(response->body->error->code)->toBe(std::string("NO_FILE"));
                expect(response->body->error->message)->toBe(std::string("No ZIP file provided"));
            }
            );
            it(std::string("should support skipValidation option"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("2.0.0")}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                zip->addFile(std::string("database/agent.json"), Buffer::from(std::string("[{"id": "test", "name": "Test"}]")));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->field(std::string("skipValidation"), std::string("true"))->attach(std::string("archive"), zipBuffer, std::string("skip-validation.zip"))->expect(200); });
                expect(response->body->status)->toBe(std::string("success"));
            }
            );
        }
        );
        describe(std::string("Complete Export/Import Flow"), [=]() mutable
        {
            it(std::string("should successfully export and re-import an agent"), [=]() mutable
            {
                vi->spyOn(fs, std::string("existsSync"))->mockReturnValue(true);
                vi->spyOn(fs, std::string("readdirSync"))->mockReturnValue(array<string>{ std::string("file1.jpg") });
                vi->spyOn(fs, std::string("statSync"))->mockReturnValue(as<any>(object{
                    object::pair{std::string("isDirectory"), [=]() mutable
                    {
                        return false;
                    }
                    }
                }));
                auto mockDb = (as<any>(mockServerInstance))["db"];
                shared queryCount = 0;
                mockDb["where"]["mockImplementation"]([=]() mutable
                {
                    queryCount++;
                    auto responses = array<array<object>>{ array<object>{ object{
                        object::pair{std::string("id"), testAgentId}, 
                        object::pair{std::string("name"), std::string("Test Agent")}, 
                        object::pair{std::string("enabled"), true}, 
                        object::pair{std::string("bio"), array<string>{ std::string("A test agent") }}, 
                        object::pair{std::string("settings"), object{
                            object::pair{std::string("theme"), std::string("dark")}
                        }}
                    } }, array<object>{ object{
                        object::pair{std::string("id"), std::string("entity-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("names"), array<string>{ std::string("Entity One") }}
                    } }, array<object>{ object{
                        object::pair{std::string("id"), std::string("world-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("name"), std::string("Test World")}
                    } }, array<object>{ object{
                        object::pair{std::string("id"), std::string("room-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("name"), std::string("Test Room")}
                    } }, array<any>(), array<object>{ object{
                        object::pair{std::string("id"), std::string("memory-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("content"), object{
                            object::pair{std::string("text"), std::string("Test memory")}
                        }}, 
                        object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                    } }, array<any>(), array<object>{ object{
                        object::pair{std::string("id"), std::string("task-1")}, 
                        object::pair{std::string("agentId"), testAgentId}, 
                        object::pair{std::string("name"), std::string("Test Task")}
                    } }, array<any>() };
                    return Promise->resolve(OR((const_(responses)[queryCount - 1]), (array<any>())));
                }
                );
                auto exportResponse = std::async([=]() { request(app)->post(std::string("/api/agents/") + testAgentId + std::string("/export"))->expect(200)->expect(std::string("Content-Type"), std::string("application/zip")); });
                expect(const_(exportResponse->headers)[std::string("x-agent-id")])->toBe(testAgentId);
                auto newAgentId = as<std::shared_ptr<UUID>>(std::string("new-agent-456"));
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("agentName"), std::string("Test Agent")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 1}, 
                    object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("elizaVersion"), std::string("1.0.0")}
                };
                zip->addFile(std::string("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                zip->addFile(std::string("database/agent.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std::string("id"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("enabled"), true}, 
                    object::pair{std::string("bio"), array<string>{ std::string("A test agent") }}, 
                    object::pair{std::string("settings"), object{
                        object::pair{std::string("theme"), std::string("dark")}
                    }}
                } }, nullptr, 2)));
                zip->addFile(std::string("database/entities.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std::string("id"), std::string("entity-1")}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("names"), array<string>{ std::string("Entity One") }}
                } }, nullptr, 2)));
                zip->addFile(std::string("database/worlds.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std::string("id"), std::string("world-1")}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test World")}
                } }, nullptr, 2)));
                zip->addFile(std::string("database/rooms.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std::string("id"), std::string("room-1")}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test Room")}
                } }, nullptr, 2)));
                zip->addFile(std::string("database/participants.json"), Buffer::from(std::string("[]")));
                zip->addFile(std::string("database/memories.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std::string("id"), std::string("memory-1")}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Test memory")}
                    }}, 
                    object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                } }, nullptr, 2)));
                zip->addFile(std::string("database/relationships.json"), Buffer::from(std::string("[]")));
                zip->addFile(std::string("database/tasks.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std::string("id"), std::string("task-1")}, 
                    object::pair{std::string("agentId"), testAgentId}, 
                    object::pair{std::string("name"), std::string("Test Task")}
                } }, nullptr, 2)));
                zip->addFile(std::string("database/server_agents.json"), Buffer::from(std::string("[]")));
                zip->addFile(std::string("uploads/file1.jpg"), Buffer::from(std::string("fake-image-data")));
                auto zipBuffer = zip->toBuffer();
                auto importResponse = std::async([=]() { request(app)->post(std::string("/api/agents/") + newAgentId + std::string("/import"))->field(std::string("overwrite"), std::string("true"))->attach(std::string("archive"), zipBuffer, std::string("exported-agent.zip"))->expect(200); });
                expect(importResponse->body->status)->toBe(std::string("success"));
                expect(importResponse->body->data->result->success)->toBe(true);
                expect(importResponse->body->data->result->agentId)->toBe(newAgentId);
                expect(importResponse->body->data->result->agentName)->toBe(std::string("Test Agent"));
                expect(importResponse->body->data->result->tablesImported)->toContain(std::string("agent"));
                expect(importResponse->body->data->result->tablesImported)->toContain(std::string("memories"));
                expect(importResponse->body->data->result->filesImported)->toBe(1);
            }
            );
        }
        );
    }
    );
}

MAIN
