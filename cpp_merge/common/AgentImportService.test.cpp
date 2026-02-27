#include "AgentImportService.test.h"

void Main(void)
{
    vi->mock(std:("adm-zip"));
    vi->mock(std:("fs"));
    vi->mock(std:("path"));
    vi->mock(std:("@elizaos/plugin-sql/schema"), [=]() mutable
    {
        return (object{
            object::pair{std:("agentTable"), object{}}, 
            object::pair{std:("memoryTable"), object{}}, 
            object::pair{std:("entityTable"), object{}}, 
            object::pair{std:("relationshipTable"), object{}}, 
            object::pair{std:("worldTable"), object{}}, 
            object::pair{std:("roomTable"), object{}}, 
            object::pair{std:("participantTable"), object{}}, 
            object::pair{std:("taskTable"), object{}}
        });
    }
    );
    vi->mock(std:("../../schema"), [=]() mutable
    {
        return (object{
            object::pair{std:("serverAgentsTable"), object{}}
        });
    }
    );
    describe(std:("AgentImportService"), [=]() mutable
    {
        shared<std::shared_ptr<AgentImportService>> importService;
        shared<std::shared_ptr<UUID>> mockTargetAgentId;
        shared<std::shared_ptr<AgentServer>> mockServerInstance;
        shared<any> mockDb;
        shared<any> mockTx;
        shared<any> mockZip;
        beforeEach([=]() mutable
        {
            mockTargetAgentId = as<std::shared_ptr<UUID>>(std:("target-agent-id"));
            mockTx = object{
                object::pair{std:("insert"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("delete"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("values"), vi->fn()->mockReturnThis()}, 
                object::pair{std:("where"), vi->fn()->mockReturnThis()}
            };
            mockDb = object{
                object::pair{std:("transaction"), vi->fn([=](auto callback) mutable
                {
                    return callback(mockTx);
                }
                )}
            };
            mockServerInstance = as<any>(object{
                object::pair{std:("db"), mockDb}
            });
            mockZip = object{
                object::pair{std:("getEntry"), vi->fn()}, 
                object::pair{std:("readAsText"), vi->fn()}, 
                object::pair{std:("getEntries"), vi->fn()->mockReturnValue(array<any>())}
            };
            (as<any>(AdmZip))["mockImplementation"]([=]() mutable
            {
                return mockZip;
            }
            );
            (as<any>(fs::existsSync))["mockReturnValue"](false);
            (as<any>(fs::mkdirSync))["mockReturnValue"](undefined);
            (as<any>(fs::writeFileSync))["mockReturnValue"](undefined);
            (as<any>(fs::rmSync))["mockReturnValue"](undefined);
            (as<any>(path->join))["mockImplementation"]([=](Args... args_) mutable
            {
                array<string> args = array<string>{args_...};
                return args->join(std:("/"));
            }
            );
            (as<any>(path->dirname))["mockImplementation"]([=](auto p) mutable
            {
                return p->substring(0, p->lastIndexOf(std:("/")));
            }
            );
            importService = std::make_shared<AgentImportService>(mockTargetAgentId, mockServerInstance);
        }
        );
        afterEach([=]() mutable
        {
            vi->clearAllMocks();
        }
        );
        describe(std:("importFromZip"), [=]() mutable
        {
            it(std:("should successfully import agent data from valid ZIP"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("original-agent-id")}, 
                    object::pair{std:("agentName"), std:("Test Agent")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 2}, 
                    object::pair{std:("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std:("elizaVersion"), std:("1.0.0")}
                };
                shared mockAgent = object{
                    object::pair{std:("id"), std:("original-agent-id")}, 
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("enabled"), true}
                };
                shared mockMemories = array<object>{ object{
                    object::pair{std:("id"), std:("memory-1")}, 
                    object::pair{std:("agentId"), std:("original-agent-id")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Test memory")}
                    }}, 
                    object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                } };
                mockZip["getEntry"]["mockImplementation"]([=](auto path) mutable
                {
                    auto entries = object{
                        object::pair{std:("manifest.json"), true}, 
                        object::pair{std:("database/agent.json"), true}, 
                        object::pair{std:("database/entities.json"), true}, 
                        object::pair{std:("database/worlds.json"), true}, 
                        object::pair{std:("database/rooms.json"), true}, 
                        object::pair{std:("database/participants.json"), true}, 
                        object::pair{std:("database/memories.json"), true}, 
                        object::pair{std:("database/relationships.json"), true}, 
                        object::pair{std:("database/tasks.json"), true}, 
                        object::pair{std:("database/server_agents.json"), true}
                    };
                    return (const_(entries)[path]) ? any(object{}) (nullptr);
                }
                );
                mockZip["readAsText"]["mockImplementation"]([=](auto entry) mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    auto data = object{
                        object::pair{std:("manifest.json"), JSON->stringify(manifest)}, 
                        object::pair{std:("database/agent.json"), JSON->stringify(array<object>{ mockAgent })}, 
                        object::pair{std:("database/entities.json"), JSON->stringify(array<any>())}, 
                        object::pair{std:("database/worlds.json"), JSON->stringify(array<any>())}, 
                        object::pair{std:("database/rooms.json"), JSON->stringify(array<any>())}, 
                        object::pair{std:("database/participants.json"), JSON->stringify(array<any>())}, 
                        object::pair{std:("database/memories.json"), JSON->stringify(mockMemories)}, 
                        object::pair{std:("database/relationships.json"), JSON->stringify(array<any>())}, 
                        object::pair{std:("database/tasks.json"), JSON->stringify(array<any>())}, 
                        object::pair{std:("database/server_agents.json"), JSON->stringify(array<any>())}
                    };
                    return OR((const_(data)[path]), (std:("[]")));
                }
                );
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std:("overwrite"), true}
                }); });
                expect(result)->toEqual(object{
                    object::pair{std:("success"), true}, 
                    object::pair{std:("agentId"), mockTargetAgentId}, 
                    object::pair{std:("agentName"), std:("Test Agent")}, 
                    object::pair{std:("tablesImported"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("filesImported"), 0}, 
                    object::pair{std:("errors"), array<any>()}
                });
                expect(mockTx["delete"])->toHaveBeenCalled();
                expect(mockTx["insert"])->toHaveBeenCalled();
            }
            );
            it(std:("should reject ZIP files that are too large"), [=]() mutable
            {
                auto largeBuffer = Buffer::alloc(600 * 1024 * 1024);
                std::async([=]() { expect(importService->importFromZip(largeBuffer))->rejects->toThrow(std:("ZIP file too large")); });
            }
            );
            it(std:("should validate manifest version"), [=]() mutable
            {
                auto manifest = object{
                    object::pair{std:("version"), std:("2.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<any>()}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockReturnValue"](JSON->stringify(manifest));
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer))->rejects->toThrow(std:("Unsupported version: 2.0.0")); });
            }
            );
            it(std:("should validate required manifest fields"), [=]() mutable
            {
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockReturnValue"](JSON->stringify(manifest));
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer))->rejects->toThrow(std:("Invalid manifest: missing required fields")); });
            }
            );
            it(std:("should check for missing expected tables"), [=]() mutable
            {
                auto manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent") }}
                };
                mockZip["getEntry"]["mockImplementation"]([=](auto path) mutable
                {
                    return (path == std:("manifest.json")) ? any(object{}) (nullptr);
                }
                );
                mockZip["readAsText"]["mockReturnValue"](JSON->stringify(manifest));
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer))->rejects->toThrow(std:("Missing expected table in export: entities")); });
            }
            );
            it(std:("should handle memories with vector embeddings"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}
                };
                shared mockMemories = array<object>{ object{
                    object::pair{std:("id"), std:("memory-1")}, 
                    object::pair{std:("agentId"), std:("original-id")}, 
                    object::pair{std:("content"), object{
                        object::pair{std:("text"), std:("Test")}
                    }}, 
                    object::pair{std:("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                } };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std:("manifest.json")) return JSON->stringify(manifest);
                    if (path == std:("database/memories.json")) return JSON->stringify(mockMemories);
                    if (path == std:("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std:("id"), std:("test")}, 
                        object::pair{std:("name"), std:("Test")}
                    } });
                    return std:("[]");
                }
                );
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std:("overwrite"), true}
                }); });
                auto memoriesInsertCall = mockTx["insert"]["mock"]["calls"]["find"]([=](auto call) mutable
                {
                    return const_(call)[0] == undefined;
                }
                );
                expect(memoriesInsertCall)->toBeDefined();
            }
            );
            it(std:("should extract uploaded files"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}, 
                    object::pair{std:("fileCount"), 2}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std:("manifest.json")) return JSON->stringify(manifest);
                    if (path == std:("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std:("id"), std:("test")}, 
                        object::pair{std:("name"), std:("Test")}
                    } });
                    return std:("[]");
                }
                );
                mockZip["getEntries"]["mockReturnValue"](array<object>{ object{
                    object::pair{std:("entryName"), std:("uploads/file1.jpg")}, 
                    object::pair{std:("isDirectory"), false}, 
                    object::pair{std:("getData"), [=]() mutable
                    {
                        return Buffer::from(std:("file1-data"));
                    }
                    }
                }, object{
                    object::pair{std:("entryName"), std:("uploads/subdir/file2.png")}, 
                    object::pair{std:("isDirectory"), false}, 
                    object::pair{std:("getData"), [=]() mutable
                    {
                        return Buffer::from(std:("file2-data"));
                    }
                    }
                }, object{
                    object::pair{std:("entryName"), std:("uploads/subdir/")}, 
                    object::pair{std:("isDirectory"), true}
                } });
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std:("overwrite"), true}
                }); });
                expect(result->filesImported)->toBe(2);
                expect(fs::writeFileSync)->toHaveBeenCalledTimes(2);
                expect(fs::mkdirSync)->toHaveBeenCalled();
            }
            );
            it(std:("should clear existing upload directory when overwriting"), [=]() mutable
            {
                (as<any>(fs::existsSync))["mockReturnValue"](true);
                shared manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std:("manifest.json")) return JSON->stringify(manifest);
                    if (path == std:("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std:("id"), std:("test")}, 
                        object::pair{std:("name"), std:("Test")}
                    } });
                    return std:("[]");
                }
                );
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std:("overwrite"), true}
                }); });
                expect(fs::rmSync)->toHaveBeenCalledWith(expect->stringContaining(mockTargetAgentId), object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
            );
            it(std:("should skip manifest validation when validateManifest is false"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std:("version"), std:("2.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<any>()}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std:("manifest.json")) return JSON->stringify(manifest);
                    if (path == std:("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std:("id"), std:("test")}, 
                        object::pair{std:("name"), std:("Test")}
                    } });
                    return std:("[]");
                }
                );
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std:("overwrite"), true}, 
                    object::pair{std:("validateManifest"), false}
                }); });
                expect(result->success)->toBe(true);
            }
            );
            it(std:("should handle transaction rollback on error"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std:("version"), std:("1.0.0")}, 
                    object::pair{std:("agentId"), std:("test")}, 
                    object::pair{std:("agentName"), std:("Test")}, 
                    object::pair{std:("tables"), array<string>{ std:("agent"), std:("entities"), std:("worlds"), std:("rooms"), std:("participants"), std:("memories"), std:("relationships"), std:("tasks"), std:("server_agents") }}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std:("manifest.json")) return JSON->stringify(manifest);
                    return std:("[]");
                }
                );
                mockDb["transaction"]["mockImplementation"]([=](auto callback) mutable
                {
                    std::async([=]() { callback(mockTx); });
                    throw any(std::make_shared<Error>(std:("Transaction failed")));
                }
                );
                auto zipBuffer = Buffer::from(std:("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer, object{
                    object::pair{std:("overwrite"), true}
                }))->rejects->toThrow(std:("Transaction failed")); });
            }
            );
        }
        );
        describe(std:("cleanup"), [=]() mutable
        {
            it(std:("should close database pool on cleanup"), [=]() mutable
            {
                auto mockPool = object{
                    object::pair{std:("end"), vi->fn()->mockResolvedValue(undefined)}
                };
                (as<any>(importService))["pool"] = mockPool;
                std::async([=]() { importService->cleanup(); });
                expect(mockPool["end"])->toHaveBeenCalled();
            }
            );
        }
        );
    }
    );
}

MAIN
