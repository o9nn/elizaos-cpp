#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/api-client/src/__tests__/services/agents.test.h"

any TEST_AGENT_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440001"));
any TEST_AGENT_ID_2 = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440002"));
any TEST_WORLD_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440003"));
any TEST_LOG_ID = as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440004"));

void Main(void)
{
    describe(std::string("AgentsService"), [=]() mutable
    {
        shared<std::shared_ptr<AgentsService>> agentsService;
        shared mockConfig = object{
            object::pair{std::string("baseUrl"), std::string("http://localhost:3000")}, 
            object::pair{std::string("apiKey"), std::string("test-key")}
        };
        beforeEach([=]() mutable
        {
            agentsService = std::make_shared<AgentsService>(mockConfig);
            (as<any>(agentsService))["get"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(agentsService))["post"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(agentsService))["put"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(agentsService))["delete"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
            (as<any>(agentsService))["patch"] = mock([=]() mutable
            {
                return Promise->resolve(object{});
            }
            );
        }
        );
        afterEach([=]() mutable
        {
            auto getMock = (as<any>(agentsService))["get"];
            auto postMock = (as<any>(agentsService))["post"];
            auto putMock = (as<any>(agentsService))["put"];
            auto deleteMock = (as<any>(agentsService))["delete"];
            auto patchMock = (as<any>(agentsService))["patch"];
            if (getMock["mockClear"]) getMock["mockClear"]();
            if (postMock["mockClear"]) postMock["mockClear"]();
            if (putMock["mockClear"]) putMock["mockClear"]();
            if (deleteMock["mockClear"]) deleteMock["mockClear"]();
            if (patchMock["mockClear"]) patchMock["mockClear"]();
        }
        );
        describe(std::string("constructor"), [=]() mutable
        {
            it(std::string("should create an instance with valid configuration"), [=]() mutable
            {
                expect(agentsService)->toBeInstanceOf(AgentsService);
            }
            );
            it(std::string("should throw error when initialized with invalid configuration"), [=]() mutable
            {
                expect([=]() mutable
                {
                    return std::make_shared<AgentsService>(as<any>(nullptr));
                }
                )->toThrow();
            }
            );
        }
        );
        describe(std::string("listAgents"), [=]() mutable
        {
            it(std::string("should retrieve agents list successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("agents"), array<object>{ object{
                        object::pair{std::string("id"), TEST_AGENT_ID}, 
                        object::pair{std::string("name"), std::string("Agent 1")}, 
                        object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("active"))}, 
                        object::pair{std::string("createdAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                        object::pair{std::string("updatedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                    }, object{
                        object::pair{std::string("id"), TEST_AGENT_ID_2}, 
                        object::pair{std::string("name"), std::string("Agent 2")}, 
                        object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("inactive"))}, 
                        object::pair{std::string("createdAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                        object::pair{std::string("updatedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                    } }}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->listAgents(); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std::string("/api/agents"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getAgent"), [=]() mutable
        {
            it(std::string("should retrieve agent successfully"), [=]() mutable
            {
                auto mockAgent = object{
                    object::pair{std::string("id"), TEST_AGENT_ID}, 
                    object::pair{std::string("name"), std::string("Test Agent")}, 
                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("active"))}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("updatedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockAgent);
                auto result = std::async([=]() { agentsService->getAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + string_empty);
                expect(result)->toEqual(mockAgent);
            }
            );
            it(std::string("should handle agent not found"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std::string("Agent not found")));
                std::async([=]() { expect(agentsService->getAgent(TEST_AGENT_ID))->rejects->toThrow(std::string("Agent not found")); });
            }
            );
        }
        );
        describe(std::string("createAgent"), [=]() mutable
        {
            shared createParams = object{
                object::pair{std::string("name"), std::string("New Agent")}, 
                object::pair{std::string("description"), std::string("A new agent")}, 
                object::pair{std::string("metadata"), object{
                    object::pair{std::string("model"), std::string("gpt-4")}
                }}
            };
            it(std::string("should create agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), TEST_AGENT_ID}, 
                    object::pair{std::string("name"), createParams["name"]}, 
                    object::pair{std::string("description"), createParams["description"]}, 
                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("active"))}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("updatedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("metadata"), createParams["metadata"]}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->createAgent(createParams); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std::string("/api/agents"), createParams);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle validation errors"), [=]() mutable
            {
                (as<any>(agentsService))["post"]["mockRejectedValue"](std::make_shared<Error>(std::string("Validation failed")));
                std::async([=]() { expect(agentsService->createAgent(createParams))->rejects->toThrow(std::string("Validation failed")); });
            }
            );
        }
        );
        describe(std::string("updateAgent"), [=]() mutable
        {
            shared updateParams = object{
                object::pair{std::string("name"), std::string("Updated Agent")}, 
                object::pair{std::string("description"), std::string("Updated description")}
            };
            it(std::string("should update agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("id"), TEST_AGENT_ID}, 
                    object::pair{std::string("name"), updateParams["name"]}, 
                    object::pair{std::string("description"), updateParams["description"]}, 
                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("active"))}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("updatedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                };
                (as<any>(agentsService))["patch"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->updateAgent(TEST_AGENT_ID, updateParams); });
                expect((as<any>(agentsService))["patch"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + string_empty, updateParams);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle partial updates"), [=]() mutable
            {
                auto partialUpdate = object{
                    object::pair{std::string("name"), std::string("New Name")}
                };
                auto mockResponse = object{
                    object::pair{std::string("id"), TEST_AGENT_ID}, 
                    object::pair{std::string("name"), partialUpdate["name"]}, 
                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("active"))}, 
                    object::pair{std::string("createdAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("updatedAt"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}
                };
                (as<any>(agentsService))["patch"]["mockResolvedValue"](mockResponse);
                std::async([=]() { agentsService->updateAgent(TEST_AGENT_ID, partialUpdate); });
                expect((as<any>(agentsService))["patch"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + string_empty, partialUpdate);
            }
            );
        }
        );
        describe(std::string("deleteAgent"), [=]() mutable
        {
            it(std::string("should delete agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(agentsService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->deleteAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["delete"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std::string("should handle deletion errors"), [=]() mutable
            {
                (as<any>(agentsService))["delete"]["mockRejectedValue"](std::make_shared<Error>(std::string("Deletion failed")));
                std::async([=]() { expect(agentsService->deleteAgent(TEST_AGENT_ID))->rejects->toThrow(std::string("Deletion failed")); });
            }
            );
        }
        );
        describe(std::string("startAgent"), [=]() mutable
        {
            it(std::string("should start agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("status"), std::string("starting")}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->startAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/start"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("stopAgent"), [=]() mutable
        {
            it(std::string("should stop agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("status"), std::string("stopped")}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->stopAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/stop"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getWorlds"), [=]() mutable
        {
            it(std::string("should get worlds successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("worlds"), array<object>{ object{
                        object::pair{std::string("id"), TEST_WORLD_ID}, 
                        object::pair{std::string("name"), std::string("World 1")}
                    }, object{
                        object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440005"))}, 
                        object::pair{std::string("name"), std::string("World 2")}
                    } }}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->getWorlds(); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std::string("/api/agents/worlds"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("addAgentToWorld"), [=]() mutable
        {
            it(std::string("should add agent to world successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->addAgentToWorld(TEST_AGENT_ID, TEST_WORLD_ID); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/worlds"), object{
                    object::pair{std::string("worldId"), TEST_WORLD_ID}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("updateAgentWorldSettings"), [=]() mutable
        {
            shared settings = object{
                object::pair{std::string("setting1"), std::string("value1")}
            };
            it(std::string("should update agent world settings successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("worldId"), TEST_WORLD_ID}, 
                    object::pair{std::string("settings"), std::string("settings")}
                };
                (as<any>(agentsService))["patch"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->updateAgentWorldSettings(TEST_AGENT_ID, TEST_WORLD_ID, settings); });
                expect((as<any>(agentsService))["patch"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/worlds/") + TEST_WORLD_ID + string_empty, object{
                    object::pair{std::string("settings"), std::string("settings")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("getAgentPanels"), [=]() mutable
        {
            it(std::string("should get agent panels successfully"), [=]() mutable
            {
                auto mockApiResponse = array<object>{ object{
                    object::pair{std::string("name"), std::string("Panel 1")}, 
                    object::pair{std::string("path"), std::string("/panel1")}
                }, object{
                    object::pair{std::string("name"), std::string("Panel 2")}, 
                    object::pair{std::string("path"), std::string("/panel2")}
                } };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockApiResponse);
                auto result = std::async([=]() { agentsService->getAgentPanels(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/panels"));
                expect(result)->toEqual(object{
                    object::pair{std::string("panels"), array<object>{ object{
                        object::pair{std::string("id"), std::string("Panel 1-0")}, 
                        object::pair{std::string("name"), std::string("Panel 1")}, 
                        object::pair{std::string("url"), std::string("/panel1")}, 
                        object::pair{std::string("type"), std::string("plugin")}
                    }, object{
                        object::pair{std::string("id"), std::string("Panel 2-1")}, 
                        object::pair{std::string("name"), std::string("Panel 2")}, 
                        object::pair{std::string("url"), std::string("/panel2")}, 
                        object::pair{std::string("type"), std::string("plugin")}
                    } }}
                });
            }
            );
        }
        );
        describe(std::string("getAgentLogs"), [=]() mutable
        {
            it(std::string("should get agent logs successfully"), [=]() mutable
            {
                auto mockLogs = array<object>{ object{
                    object::pair{std::string("id"), TEST_LOG_ID}, 
                    object::pair{std::string("agentId"), TEST_AGENT_ID}, 
                    object::pair{std::string("timestamp"), std::make_shared<Date>(std::string("2024-01-01T00:00:00Z"))}, 
                    object::pair{std::string("level"), as<std::shared_ptr<const>>(std::string("info"))}, 
                    object::pair{std::string("message"), std::string("Agent started")}
                }, object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("550e8400-e29b-41d4-a716-446655440006"))}, 
                    object::pair{std::string("agentId"), TEST_AGENT_ID}, 
                    object::pair{std::string("timestamp"), std::make_shared<Date>(std::string("2024-01-01T00:01:00Z"))}, 
                    object::pair{std::string("level"), as<std::shared_ptr<const>>(std::string("debug"))}, 
                    object::pair{std::string("message"), std::string("Processing message")}
                } };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockLogs);
                auto result = std::async([=]() { agentsService->getAgentLogs(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/logs"), object{
                    object::pair{std::string("params"), undefined}
                });
                expect(result)->toEqual(mockLogs);
            }
            );
            it(std::string("should handle log parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std::string("limit"), 100}, 
                    object::pair{std::string("level"), as<std::shared_ptr<const>>(std::string("error"))}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](array<any>());
                std::async([=]() { agentsService->getAgentLogs(TEST_AGENT_ID, params); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/logs"), object{
                    object::pair{std::string("params"), std::string("params")}
                });
            }
            );
        }
        );
        describe(std::string("deleteAgentLog"), [=]() mutable
        {
            it(std::string("should delete agent log successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std::string("success"), true}
                };
                (as<any>(agentsService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->deleteAgentLog(TEST_AGENT_ID, TEST_LOG_ID); });
                expect((as<any>(agentsService))["delete"])->toHaveBeenCalledWith(std::string("/api/agents/") + TEST_AGENT_ID + std::string("/logs/") + TEST_LOG_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std::string("error handling"), [=]() mutable
        {
            it(std::string("should handle network errors"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std::string("Network error")));
                std::async([=]() { expect(agentsService->listAgents())->rejects->toThrow(std::string("Network error")); });
            }
            );
            it(std::string("should handle API errors"), [=]() mutable
            {
                (as<any>(agentsService))["post"]["mockRejectedValue"](std::make_shared<Error>(std::string("API error")));
                std::async([=]() { expect(agentsService->createAgent(object{
                    object::pair{std::string("name"), std::string("test")}
                }))->rejects->toThrow(std::string("API error")); });
            }
            );
            it(std::string("should handle unauthorized errors"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std::string("Unauthorized")));
                std::async([=]() { expect(agentsService->getAgent(TEST_AGENT_ID))->rejects->toThrow(std::string("Unauthorized")); });
            }
            );
            it(std::string("should handle rate limiting"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std::string("Rate limit exceeded")));
                std::async([=]() { expect(agentsService->listAgents())->rejects->toThrow(std::string("Rate limit exceeded")); });
            }
            );
        }
        );
    }
    );
}

MAIN
