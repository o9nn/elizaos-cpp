#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/services/AgentImportService.test.h"

void Main(void)
{
    vi->mock(std::string("adm-zip"));
    vi->mock(std::string("fs"));
    vi->mock(std::string("path"));
    vi->mock(std::string("@elizaos/plugin-sql/schema"), [=]() mutable
    {
        return (object{
            object::pair{std::string("agentTable"), object{}}, 
            object::pair{std::string("memoryTable"), object{}}, 
            object::pair{std::string("entityTable"), object{}}, 
            object::pair{std::string("relationshipTable"), object{}}, 
            object::pair{std::string("worldTable"), object{}}, 
            object::pair{std::string("roomTable"), object{}}, 
            object::pair{std::string("participantTable"), object{}}, 
            object::pair{std::string("taskTable"), object{}}
        });
    }
    );
    vi->mock(std::string("../../schema"), [=]() mutable
    {
        return (object{
            object::pair{std::string("serverAgentsTable"), object{}}
        });
    }
    );
    describe(std::string("AgentImportService"), [=]() mutable
    {
        shared<std::shared_ptr<AgentImportService>> importService;
        shared<std::shared_ptr<UUID>> mockTargetAgentId;
        shared<std::shared_ptr<AgentServer>> mockServerInstance;
        shared<any> mockDb;
        shared<any> mockTx;
        shared<any> mockZip;
        beforeEach([=]() mutable
        {
            mockTargetAgentId = as<std::shared_ptr<UUID>>(std::string("target-agent-id"));
            mockTx = object{
                object::pair{std::string("insert"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("delete"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("values"), vi->fn()->mockReturnThis()}, 
                object::pair{std::string("where"), vi->fn()->mockReturnThis()}
            };
            mockDb = object{
                object::pair{std::string("transaction"), vi->fn([=](auto callback) mutable
                {
                    return callback(mockTx);
                }
                )}
            };
            mockServerInstance = as<any>(object{
                object::pair{std::string("db"), mockDb}
            });
            mockZip = object{
                object::pair{std::string("getEntry"), vi->fn()}, 
                object::pair{std::string("readAsText"), vi->fn()}, 
                object::pair{std::string("getEntries"), vi->fn()->mockReturnValue(array<any>())}
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
                return args->join(std::string("/"));
            }
            );
            (as<any>(path->dirname))["mockImplementation"]([=](auto p) mutable
            {
                return p->substring(0, p->lastIndexOf(std::string("/")));
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
        describe(std::string("importFromZip"), [=]() mutable
        {
            it(std::string("should successfully import agent data from valid ZIP"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("original-agent-id")}, 
                    object::pair{std::string("agentName"), std::string("Test Agent")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 2}, 
                    object::pair{std::string("exportedAt"), ((std::make_shared<Date>()))->toISOString()}, 
                    object::pair{std::string("elizaVersion"), std::string("1.0.0")}
                };
                shared mockAgent = object{
                    object::pair{std::string("id"), std::string("original-agent-id")}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("enabled"), true}
                };
                shared mockMemories = array<object>{ object{
                    object::pair{std::string("id"), std::string("memory-1")}, 
                    object::pair{std::string("agentId"), std::string("original-agent-id")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Test memory")}
                    }}, 
                    object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                } };
                mockZip["getEntry"]["mockImplementation"]([=](auto path) mutable
                {
                    auto entries = object{
                        object::pair{std::string("manifest.json"), true}, 
                        object::pair{std::string("database/agent.json"), true}, 
                        object::pair{std::string("database/entities.json"), true}, 
                        object::pair{std::string("database/worlds.json"), true}, 
                        object::pair{std::string("database/rooms.json"), true}, 
                        object::pair{std::string("database/participants.json"), true}, 
                        object::pair{std::string("database/memories.json"), true}, 
                        object::pair{std::string("database/relationships.json"), true}, 
                        object::pair{std::string("database/tasks.json"), true}, 
                        object::pair{std::string("database/server_agents.json"), true}
                    };
                    return (const_(entries)[path]) ? any(object{}) : any(nullptr);
                }
                );
                mockZip["readAsText"]["mockImplementation"]([=](auto entry) mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    auto data = object{
                        object::pair{std::string("manifest.json"), JSON->stringify(manifest)}, 
                        object::pair{std::string("database/agent.json"), JSON->stringify(array<object>{ mockAgent })}, 
                        object::pair{std::string("database/entities.json"), JSON->stringify(array<any>())}, 
                        object::pair{std::string("database/worlds.json"), JSON->stringify(array<any>())}, 
                        object::pair{std::string("database/rooms.json"), JSON->stringify(array<any>())}, 
                        object::pair{std::string("database/participants.json"), JSON->stringify(array<any>())}, 
                        object::pair{std::string("database/memories.json"), JSON->stringify(mockMemories)}, 
                        object::pair{std::string("database/relationships.json"), JSON->stringify(array<any>())}, 
                        object::pair{std::string("database/tasks.json"), JSON->stringify(array<any>())}, 
                        object::pair{std::string("database/server_agents.json"), JSON->stringify(array<any>())}
                    };
                    return OR((const_(data)[path]), (std::string("[]")));
                }
                );
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std::string("overwrite"), true}
                }); });
                expect(result)->toEqual(object{
                    object::pair{std::string("success"), true}, 
                    object::pair{std::string("agentId"), mockTargetAgentId}, 
                    object::pair{std::string("agentName"), std::string("Test Agent")}, 
                    object::pair{std::string("tablesImported"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("filesImported"), 0}, 
                    object::pair{std::string("errors"), array<any>()}
                });
                expect(mockTx["delete"])->toHaveBeenCalled();
                expect(mockTx["insert"])->toHaveBeenCalled();
            }
            );
            it(std::string("should reject ZIP files that are too large"), [=]() mutable
            {
                auto largeBuffer = Buffer::alloc(600 * 1024 * 1024);
                std::async([=]() { expect(importService->importFromZip(largeBuffer))->rejects->toThrow(std::string("ZIP file too large")); });
            }
            );
            it(std::string("should validate manifest version"), [=]() mutable
            {
                auto manifest = object{
                    object::pair{std::string("version"), std::string("2.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<any>()}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockReturnValue"](JSON->stringify(manifest));
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer))->rejects->toThrow(std::string("Unsupported export version: 2.0.0")); });
            }
            );
            it(std::string("should validate required manifest fields"), [=]() mutable
            {
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockReturnValue"](JSON->stringify(manifest));
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer))->rejects->toThrow(std::string("Invalid manifest: missing required fields")); });
            }
            );
            it(std::string("should check for missing expected tables"), [=]() mutable
            {
                auto manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent") }}
                };
                mockZip["getEntry"]["mockImplementation"]([=](auto path) mutable
                {
                    return (path == std::string("manifest.json")) ? any(object{}) : any(nullptr);
                }
                );
                mockZip["readAsText"]["mockReturnValue"](JSON->stringify(manifest));
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer))->rejects->toThrow(std::string("Missing expected table in export: entities")); });
            }
            );
            it(std::string("should handle memories with vector embeddings"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}
                };
                shared mockMemories = array<object>{ object{
                    object::pair{std::string("id"), std::string("memory-1")}, 
                    object::pair{std::string("agentId"), std::string("original-id")}, 
                    object::pair{std::string("content"), object{
                        object::pair{std::string("text"), std::string("Test")}
                    }}, 
                    object::pair{std::string("embedding"), array<double>{ 0.1, 0.2, 0.3 }}
                } };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std::string("manifest.json")) return JSON->stringify(manifest);
                    if (path == std::string("database/memories.json")) return JSON->stringify(mockMemories);
                    if (path == std::string("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std::string("id"), std::string("test")}, 
                        object::pair{std::string("name"), std::string("Test")}
                    } });
                    return std::string("[]");
                }
                );
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std::string("overwrite"), true}
                }); });
                auto memoriesInsertCall = mockTx["insert"]["mock"]["calls"]["find"]([=](auto call) mutable
                {
                    return const_(call)[0] == undefined;
                }
                );
                expect(memoriesInsertCall)->toBeDefined();
            }
            );
            it(std::string("should extract uploaded files"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}, 
                    object::pair{std::string("fileCount"), 2}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std::string("manifest.json")) return JSON->stringify(manifest);
                    if (path == std::string("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std::string("id"), std::string("test")}, 
                        object::pair{std::string("name"), std::string("Test")}
                    } });
                    return std::string("[]");
                }
                );
                mockZip["getEntries"]["mockReturnValue"](array<object>{ object{
                    object::pair{std::string("entryName"), std::string("uploads/file1.jpg")}, 
                    object::pair{std::string("isDirectory"), false}, 
                    object::pair{std::string("getData"), [=]() mutable
                    {
                        return Buffer::from(std::string("file1-data"));
                    }
                    }
                }, object{
                    object::pair{std::string("entryName"), std::string("uploads/subdir/file2.png")}, 
                    object::pair{std::string("isDirectory"), false}, 
                    object::pair{std::string("getData"), [=]() mutable
                    {
                        return Buffer::from(std::string("file2-data"));
                    }
                    }
                }, object{
                    object::pair{std::string("entryName"), std::string("uploads/subdir/")}, 
                    object::pair{std::string("isDirectory"), true}
                } });
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std::string("overwrite"), true}
                }); });
                expect(result->filesImported)->toBe(2);
                expect(fs::writeFileSync)->toHaveBeenCalledTimes(2);
                expect(fs::mkdirSync)->toHaveBeenCalled();
            }
            );
            it(std::string("should clear existing upload directory when overwriting"), [=]() mutable
            {
                (as<any>(fs::existsSync))["mockReturnValue"](true);
                shared manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std::string("manifest.json")) return JSON->stringify(manifest);
                    if (path == std::string("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std::string("id"), std::string("test")}, 
                        object::pair{std::string("name"), std::string("Test")}
                    } });
                    return std::string("[]");
                }
                );
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std::string("overwrite"), true}
                }); });
                expect(fs::rmSync)->toHaveBeenCalledWith(expect->stringContaining(mockTargetAgentId), object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
            );
            it(std::string("should skip manifest validation when validateManifest is false"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std::string("version"), std::string("2.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<any>()}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std::string("manifest.json")) return JSON->stringify(manifest);
                    if (path == std::string("database/agent.json")) return JSON->stringify(array<object>{ object{
                        object::pair{std::string("id"), std::string("test")}, 
                        object::pair{std::string("name"), std::string("Test")}
                    } });
                    return std::string("[]");
                }
                );
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                auto result = std::async([=]() { importService->importFromZip(zipBuffer, object{
                    object::pair{std::string("overwrite"), true}, 
                    object::pair{std::string("validateManifest"), false}
                }); });
                expect(result->success)->toBe(true);
            }
            );
            it(std::string("should handle transaction rollback on error"), [=]() mutable
            {
                shared manifest = object{
                    object::pair{std::string("version"), std::string("1.0.0")}, 
                    object::pair{std::string("agentId"), std::string("test")}, 
                    object::pair{std::string("agentName"), std::string("Test")}, 
                    object::pair{std::string("tables"), array<string>{ std::string("agent"), std::string("entities"), std::string("worlds"), std::string("rooms"), std::string("participants"), std::string("memories"), std::string("relationships"), std::string("tasks"), std::string("server_agents") }}
                };
                mockZip["getEntry"]["mockReturnValue"](object{});
                mockZip["readAsText"]["mockImplementation"]([=]() mutable
                {
                    auto path = const_(const_(mockZip["getEntry"]["mock"]["calls"])[mockZip["getEntry"]["mock"]["calls"]["length"] - 1])[0];
                    if (path == std::string("manifest.json")) return JSON->stringify(manifest);
                    return std::string("[]");
                }
                );
                mockDb["transaction"]["mockImplementation"]([=](auto callback) mutable
                {
                    std::async([=]() { callback(mockTx); });
                    throw any(std::make_shared<Error>(std::string("Transaction failed")));
                }
                );
                auto zipBuffer = Buffer::from(std::string("mock-zip-data"));
                std::async([=]() { expect(importService->importFromZip(zipBuffer, object{
                    object::pair{std::string("overwrite"), true}
                }))->rejects->toThrow(std::string("Transaction failed")); });
            }
            );
        }
        );
        describe(std::string("cleanup"), [=]() mutable
        {
            it(std::string("should close database pool on cleanup"), [=]() mutable
            {
                auto mockPool = object{
                    object::pair{std::string("end"), vi->fn()->mockResolvedValue(undefined)}
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
