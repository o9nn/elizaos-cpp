#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/integration/agent-portability.test.h"

void Main(void)
{
    describe(std:("Agent Portability Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<express::Application>> app;
        shared<std::shared_ptr<Map<std::shared_ptr<UUID>, std::shared_ptr<IAgentRuntime>>>> agents;
        shared<std::shared_ptr<AgentServer>> mockServerInstance;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared testAgentId = as<std::shared_ptr<UUID>>(std:("test-agent-123"));
        beforeEach([=]() mutable
        {
            app = express();
            agents = std::make_shared<Map>();
            mockRuntime = as<std::shared_ptr<IAgentRuntime>>(object{
                object::pair{std:("agentId"), testAgentId}, 
                object::pair{std:("character"), object{
                    object::pair{std:("name"), std:("Test Agent")}
                }}
            });
            agents->set(testAgentId, mockRuntime);
            auto mockDb = object{
                object::pair{std:("select"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("from"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("where"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("insert"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("delete"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("values"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("transaction"), vi->fn([=](auto callback) mutable
                {
                    return callback(object{
                        object::pair{std:("insert"), vi->fn()->mockReturnThis()}, 
                        object::pair{std:("delete"), vi->fn()->mockReturnThis()}, 
                        object::pair{std:("values"), vi->fn()->mockReturnThis()}, 
                        object::pair{std:("where"), vi->fn()->mockReturnThis()}
                    });
                }
                )}
            };
            mockDb["where"]->mockResolvedValue(array<object>{ object{
                object::pair{std:("id"), testAgentId}, 
                object::pair{std:("name"), std:("Test Agent")}, 
                object::pair{std:("enabled"), true}, 
                object::pair{std:("createdAt"), std::make_shared<Date>()}, 
                object::pair{std:("updatedAt"), std::make_shared<Date>()}
            } });
            mockServerInstance = as<any>(object{
                object::pair{std:("db"), mockDb}, 
                object::pair{std:("getAgents"), [=]() mutable
                {
                    return agents;
                }
                }
            });
            app->use(std:("/api"), createApiRouter(agents, mockServerInstance));
        }
        );
        afterEach([=]() mutable
        {
            vi->clearAllMocks();
        }
        );
        describe(std:("Export/Import Cycle"), [=]() mutable
        {
            it(std:("should an agent to ZIP file"), [=]() mutable
            {
                vi->spyOn(fs, std:("existsSync"))->mockReturnValue(false);
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/export"))->expect(200)->expect(std:("Content-Type"), std:("application/zip")); });
                expect(const_(response->headers)[std:("content-disposition")])->toMatch((new RegExp(std:("attachment; filename="agent-export"))));
                expect(const_(response->headers)[std:("x-agent-id")])->toBe(testAgentId);
            }
            );
            it(std:("should validate ZIP file before import"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("original-id")}, 
                    object::pair{std:("agentName"), std:("Test Agent")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 0}, 
                    object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("elizaVersion"), std:("1.0.0")}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                auto tables = array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") };
                for (auto& table : tables)
                {
                    zip->addFile(std:("database/") + table + std:(".json"), Buffer::from(std:("[]")));
                }
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import/validate"))->attach(std:("archive"), zipBuffer, std:("test-export.zip"))->expect(200); });
                expect(response->body->status)->toBe(std:("success"));
                expect(response->body->data->result->valid)->toBe(true);
                expect(response->body->data->result->manifest->agentName)->toBe(std:("Test Agent"));
            }
            );
            it(std:("should reject import without overwrite flag if agent exists"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                zip->addFile(std:("manifest.json"), Buffer::from(std:("{}")));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("false"))->attach(std:("archive"), zipBuffer, std:("test-export.zip"))->expect(409); });
                expect(response->body->error->code)->toBe(std:("AGENT_EXISTS"));
                expect(response->body->error->message)->toContain(std:("Set overwrite=true"));
            }
            );
            it(std:("should import agent data with overwrite flag"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("original-id")}, 
                    object::pair{std:("agentName"), std:("Imported Agent")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 0}, 
                    object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("elizaVersion"), std:("1.0.0")}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                auto agentData = array<object>{ object{
                    object::pair{std:("id"), std:("original-id")}, 
                    object::pair{std:("name"), std:("Imported Agent")}, 
                    object::pair{std:("enabled"), true}, 
                    object::pair{std:("createdAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("updatedAt"), ((std::make_shared<Date>()))->toISOString()}
                } };
                zip->addFile(std:("database/agent.json"), Buffer::from(JSON->stringify(agentData, nullptr, 2)));
                auto emptyTables = array<string>{ std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") };
                for (auto& table : emptyTables)
                {
                    zip->addFile(std:("database/") + table + std:(".json"), Buffer::from(std:("[]")));
                }
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->attach(std:("archive"), zipBuffer, std:("test-export.zip"))->expect(200); });
                expect(response->body->status)->toBe(std:("success"));
                expect(response->body->data->result->success)->toBe(true);
                expect(response->body->data->result->agentName)->toBe(std:("Imported Agent"));
                expect(response->body->data->result->tablesImported)->toHaveLength(9);
            }
            );
            it(std:("should handle invalid ZIP file gracefully"), [=]() mutable
            {
                auto invalidZipBuffer = Buffer::from(std:("This is not a valid ZIP file"));
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->attach(std:("archive"), invalidZipBuffer, std:("invalid.zip"))->expect(500); });
                expect(response->body->error->code)->toBe(std:("IMPORT_FAILED"));
            }
            );
            it(std:("should reject oversized ZIP files"), [=]() mutable
            {
                auto largeBuffer = Buffer::alloc(600 * 1024 * 1024);
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->attach(std:("archive"), largeBuffer, std:("large.zip"))->expect(413); });
                expect(response->body->error)->toBeDefined();
            }
            );
            it(std:("should handle missing manifest.json"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                zip->addFile(std:("database/agent.json"), Buffer::from(std:("[]")));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->attach(std:("archive"), zipBuffer, std:("no-manifest.zip"))->expect(400); });
                expect(response->body->error->code)->toBe(std:("INVALID_ARCHIVE"));
                expect(response->body->error->message)->toContain(std:("manifest.json not found"));
            }
            );
            it(std:("should validate manifest version"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("99.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<any>()}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->attach(std:("archive"), zipBuffer, std:("wrong-version.zip"))->expect(400); });
                expect(response->body->error->code)->toBe(std:("VERSION_MISMATCH"));
                expect(response->body->error->message)->toContain(std:("Unsupported version"));
            }
            );
            it(std:("should handle errors gracefully"), [=]() mutable
            {
                agents->clear();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/export"))->expect(404); });
                expect(response->body->error->code)->toBe(std:("AGENT_NOT_FOUND"));
            }
            );
            it(std:("should validate agent ID format"), [=]() mutable
            {
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/invalid-uuid/export"))->expect(400); });
                expect(response->body->error->code)->toBe(std:("INVALID_AGENT_ID"));
            }
            );
            it(std:("should require ZIP file for import"), [=]() mutable
            {
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->expect(400); });
                expect(response->body->error->code)->toBe(std:("NO_FILE"));
                expect(response->body->error->message)->toBe(std:("No ZIP file provided"));
            }
            );
            it(std:("should support skipValidation option"), [=]() mutable
            {
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("2.0.0")}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest)));
                zip->addFile(std:("database/agent.json"), Buffer::from(std:("[{"id": "test", "name": "Test"}]")));
                auto zipBuffer = zip->toBuffer();
                auto response = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->field(std:("skipValidation"), std:("true"))->attach(std:("archive"), zipBuffer, std:("skip-validation.zip"))->expect(200); });
                expect(response->body->status)->toBe(std:("success"));
            }
            );
        }
        );
        describe(std:("Complete Export/Import Flow"), [=]() mutable
        {
            it(std:("should successfully and re-import an agent"), [=]() mutable
            {
                vi->spyOn(fs, std:("existsSync"))->mockReturnValue(true);
                vi->spyOn(fs, std:("readdirSync"))->mockReturnValue(array<string>{ std:("file1.jpg") });
                vi->spyOn(fs, std:("statSync"))->mockReturnValue(as<any>(object{
                    object::pair{std:("isDirectory"), [=]() mutable
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
                        object::pair{std:("id"), testAgentId}, 
                        object::pair{std:("name"), std:("Test Agent")}, 
                        object::pair{std:("enabled"), true}, 
                        object::pair{std:("bio"), array<string>{ std:("A test agent") }}, 
                        object::pair{std:("settings"), object{
                            object::pair{std:("theme"), std:("dark")}
                        }}
                    } }, array<object>{ object{
                        object::pair{std:("id"), std:("entity-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("names"), array<string>{ std:("Entity One") }}
                    } }, array<object>{ object{
                        object::pair{std:("id"), std:("world-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("name"), std:("Test World")}
                    } }, array<object>{ object{
                        object::pair{std:("id"), std:("room-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("name"), std:("Test Room")}
                    } }, array<any>(), array<object>{ object{
                        object::pair{std:("id"), std:("memory-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("content"), object{
                            object::pair{std:("text"), std:("Test memory")}
                        }}, 
                        object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                    } }, array<any>(), array<object>{ object{
                        object::pair{std:("id"), std:("task-1")}, 
                        object::pair{std:("agentId"), testAgentId}, 
                        object::pair{std:("name"), std:("Test Task")}
                    } }, array<any>() };
                    return Promise->resolve(OR((const_(responses)[queryCount - 1]), (array<any>())));
                }
                );
                auto exportResponse = std::async([=]() { request(app)->post(std:("/api/agents/") + testAgentId + std:("/export"))->expect(200)->expect(std:("Content-Type"), std:("application/zip")); });
                expect(const_(exportResponse->headers)[std:("x-agent-id")])->toBe(testAgentId);
                auto newAgentId = as<std::shared_ptr<UUID>>(std:("new-agent-456"));
                auto zip = std::make_shared<AdmZip>();
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("agentName"), std:("Test Agent")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 1}, 
                    object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("elizaVersion"), std:("1.0.0")}
                };
                zip->addFile(std:("manifest.json"), Buffer::from(JSON->stringify(manifest, nullptr, 2)));
                zip->addFile(std:("database/agent.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std:("id"), testAgentId}, 
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("enabled"), true}, 
                    object::pair{std:("bio"), array<string>{ std:("A test agent") }}, 
                    object::pair{std:("settings"), object{
                        object::pair{std:("theme"), std:("dark")}
                    }}
                } }, nullptr, 2)));
                zip->addFile(std:("database/entities.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std:("id"), std:("entity-1")}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("names"), array<string>{ std:("Entity One") }}
                } }, nullptr, 2)));
                zip->addFile(std:("database/worlds.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std:("id"), std:("world-1")}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Test World")}
                } }, nullptr, 2)));
                zip->addFile(std:("database/rooms.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std:("id"), std:("room-1")}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Test Room")}
                } }, nullptr, 2)));
                zip->addFile(std:("database/participants.json"), Buffer::from(std:("[]")));
                zip->addFile(std:("database/memories.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std:("id"), std:("memory-1")}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Test memory")}
                    }}, 
                    object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                } }, nullptr, 2)));
                zip->addFile(std:("database/relationships.json"), Buffer::from(std:("[]")));
                zip->addFile(std:("database/tasks.json"), Buffer::from(JSON->stringify(array<object>{ object{
                    object::pair{std:("id"), std:("task-1")}, 
                    object::pair{std:("agentId"), testAgentId}, 
                    object::pair{std:("name"), std:("Test Task")}
                } }, nullptr, 2)));
                zip->addFile(std:("database/server_agents.json"), Buffer::from(std:("[]")));
                zip->addFile(std:("uploads/file1.jpg"), Buffer::from(std:("fake-image-data")));
                auto zipBuffer = zip->toBuffer();
                auto importResponse = std::async([=]() { request(app)->post(std:("/api/agents/") + newAgentId + std:("/import"))->field(std:("overwrite"), std:("true"))->attach(std:("archive"), zipBuffer, std:("exported-agent.zip"))->expect(200); });
                expect(importResponse->body->status)->toBe(std:("success"));
                expect(importResponse->body->data->result->success)->toBe(true);
                expect(importResponse->body->data->result->agentId)->toBe(newAgentId);
                expect(importResponse->body->data->result->agentName)->toBe(std:("Test Agent"));
                expect(importResponse->body->data->result->tablesImported)->toContain(std:("agent"));
                expect(importResponse->body->data->result->tablesImported)->toContain(std:("memories"));
                expect(importResponse->body->data->result->filesImported)->toBe(1);
            }
            );
        }
        );
    }
    );
}

MAIN
