#include "agents.test.h"

any TEST_AGENT_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440001"));
any TEST_AGENT_ID_2 = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440002"));
any TEST_WORLD_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440003"));
any TEST_LOG_ID = as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440004"));

void Main(void)
{
    describe(std:("AgentsService"), [=]() mutable
    {
        shared<std::shared_ptr<AgentsService>> agentsService;
        shared mockConfig = object{
            object::pair{std:("baseUrl"), std:("http://localhost:3000")}, 
            object::pair{std:("apiKey"), std:("test-key")}
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
        describe(std:("constructor"), [=]() mutable
        {
            it(std:("should create an instance with valid configuration"), [=]() mutable
            {
                expect(agentsService)->toBeInstanceOf(AgentsService);
            }
            );
            it(std:("should throw error when initialized with invalid configuration"), [=]() mutable
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
        describe(std:("listAgents"), [=]() mutable
        {
            it(std:("should retrieve agents list successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("agents"), array<object>{ object{
                        object::pair{std:("id"), TEST_AGENT_ID}, 
                        object::pair{std:("name"), std:("Agent 1")}, 
                        object::pair{std:("status"), as<std::shared_ptr<const>>(std:("active"))}, 
                        object::pair{std:("createdAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                        object::pair{std:("updatedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                    }, object{
                        object::pair{std:("id"), TEST_AGENT_ID_2}, 
                        object::pair{std:("name"), std:("Agent 2")}, 
                        object::pair{std:("status"), as<std::shared_ptr<const>>(std:("inactive"))}, 
                        object::pair{std:("createdAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                        object::pair{std:("updatedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                    } }}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->listAgents(); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std:("/api/agents"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getAgent"), [=]() mutable
        {
            it(std:("should retrieve agent successfully"), [=]() mutable
            {
                auto mockAgent = object{
                    object::pair{std:("id"), TEST_AGENT_ID}, 
                    object::pair{std:("name"), std:("Test Agent")}, 
                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("active"))}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("updatedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockAgent);
                auto result = std::async([=]() { agentsService->getAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + string_empty);
                expect(result)->toEqual(mockAgent);
            }
            );
            it(std:("should handle agent not found"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std:("Agent not found")));
                std::async([=]() { expect(agentsService->getAgent(TEST_AGENT_ID))->rejects->toThrow(std:("Agent not found")); });
            }
            );
        }
        );
        describe(std:("createAgent"), [=]() mutable
        {
            shared createParams = object{
                object::pair{std:("name"), std:("New Agent")}, 
                object::pair{std:("description"), std:("A new agent")}, 
                object::pair{std:("metadata"), object{
                    object::pair{std:("model"), std:("gpt-4")}
                }}
            };
            it(std:("should create agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), TEST_AGENT_ID}, 
                    object::pair{std:("name"), createParams["name"]}, 
                    object::pair{std:("description"), createParams["description"]}, 
                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("active"))}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("updatedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("metadata"), createParams["metadata"]}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->createAgent(createParams); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std:("/api/agents"), createParams);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle validation errors"), [=]() mutable
            {
                (as<any>(agentsService))["post"]["mockRejectedValue"](std::make_shared<Error>(std:("Validation failed")));
                std::async([=]() { expect(agentsService->createAgent(createParams))->rejects->toThrow(std:("Validation failed")); });
            }
            );
        }
        );
        describe(std:("updateAgent"), [=]() mutable
        {
            shared updateParams = object{
                object::pair{std:("name"), std:("Updated Agent")}, 
                object::pair{std:("description"), std:("Updated description")}
            };
            it(std:("should update agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("id"), TEST_AGENT_ID}, 
                    object::pair{std:("name"), updateParams["name"]}, 
                    object::pair{std:("description"), updateParams["description"]}, 
                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("active"))}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("updatedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                };
                (as<any>(agentsService))["patch"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->updateAgent(TEST_AGENT_ID, updateParams); });
                expect((as<any>(agentsService))["patch"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + string_empty, updateParams);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle partial updates"), [=]() mutable
            {
                auto partialUpdate = object{
                    object::pair{std:("name"), std:("New Name")}
                };
                auto mockResponse = object{
                    object::pair{std:("id"), TEST_AGENT_ID}, 
                    object::pair{std:("name"), partialUpdate["name"]}, 
                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("active"))}, 
                    object::pair{std:("createdAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("updatedAt"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}
                };
                (as<any>(agentsService))["patch"]["mockResolvedValue"](mockResponse);
                std::async([=]() { agentsService->updateAgent(TEST_AGENT_ID, partialUpdate); });
                expect((as<any>(agentsService))["patch"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + string_empty, partialUpdate);
            }
            );
        }
        );
        describe(std:("deleteAgent"), [=]() mutable
        {
            it(std:("should delete agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(agentsService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->deleteAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["delete"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
            it(std:("should handle deletion errors"), [=]() mutable
            {
                (as<any>(agentsService))["delete"]["mockRejectedValue"](std::make_shared<Error>(std:("Deletion failed")));
                std::async([=]() { expect(agentsService->deleteAgent(TEST_AGENT_ID))->rejects->toThrow(std:("Deletion failed")); });
            }
            );
        }
        );
        describe(std:("startAgent"), [=]() mutable
        {
            it(std:("should start agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("status"), std:("starting")}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->startAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/start"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("stopAgent"), [=]() mutable
        {
            it(std:("should stop agent successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("status"), std:("stopped")}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->stopAgent(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/stop"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getWorlds"), [=]() mutable
        {
            it(std:("should get worlds successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("worlds"), array<object>{ object{
                        object::pair{std:("id"), TEST_WORLD_ID}, 
                        object::pair{std:("name"), std:("World 1")}
                    }, object{
                        object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440005"))}, 
                        object::pair{std:("name"), std:("World 2")}
                    } }}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->getWorlds(); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std:("/api/agents/worlds"));
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("addAgentToWorld"), [=]() mutable
        {
            it(std:("should add agent to world successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(agentsService))["post"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->addAgentToWorld(TEST_AGENT_ID, TEST_WORLD_ID); });
                expect((as<any>(agentsService))["post"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/worlds"), object{
                    object::pair{std:("worldId"), TEST_WORLD_ID}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("updateAgentWorldSettings"), [=]() mutable
        {
            shared settings = object{
                object::pair{std:("setting1"), std:("value1")}
            };
            it(std:("should update agent world settings successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("worldId"), TEST_WORLD_ID}, 
                    object::pair{std:("settings"), std:("settings")}
                };
                (as<any>(agentsService))["patch"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->updateAgentWorldSettings(TEST_AGENT_ID, TEST_WORLD_ID, settings); });
                expect((as<any>(agentsService))["patch"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/worlds/") + TEST_WORLD_ID + string_empty, object{
                    object::pair{std:("settings"), std:("settings")}
                });
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("getAgentPanels"), [=]() mutable
        {
            it(std:("should get agent panels successfully"), [=]() mutable
            {
                auto mockApiResponse = array<object>{ object{
                    object::pair{std:("name"), std:("Panel 1")}, 
                    object::pair{std:("path"), std:("/panel1")}
                }, object{
                    object::pair{std:("name"), std:("Panel 2")}, 
                    object::pair{std:("path"), std:("/panel2")}
                } };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockApiResponse);
                auto result = std::async([=]() { agentsService->getAgentPanels(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/panels"));
                expect(result)->toEqual(object{
                    object::pair{std:("panels"), array<object>{ object{
                        object::pair{std:("id"), std:("Panel 1-0")}, 
                        object::pair{std:("name"), std:("Panel 1")}, 
                        object::pair{std:("url"), std:("/panel1")}, 
                        object::pair{std:("type"), std:("plugin")}
                    }, object{
                        object::pair{std:("id"), std:("Panel 2-1")}, 
                        object::pair{std:("name"), std:("Panel 2")}, 
                        object::pair{std:("url"), std:("/panel2")}, 
                        object::pair{std:("type"), std:("plugin")}
                    } }}
                });
            }
            );
        }
        );
        describe(std:("getAgentLogs"), [=]() mutable
        {
            it(std:("should get agent logs successfully"), [=]() mutable
            {
                auto mockLogs = array<object>{ object{
                    object::pair{std:("id"), TEST_LOG_ID}, 
                    object::pair{std:("agentId"), TEST_AGENT_ID}, 
                    object::pair{std:("timestamp"), std::make_shared<Date>(std:("2024-01-01T00:00:00Z"))}, 
                    object::pair{std:("level"), as<std::shared_ptr<const>>(std:("info"))}, 
                    object::pair{std:("message"), std:("Agent started")}
                }, object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("550e8400-e29b-41d4-a716-446655440006"))}, 
                    object::pair{std:("agentId"), TEST_AGENT_ID}, 
                    object::pair{std:("timestamp"), std::make_shared<Date>(std:("2024-01-01T00:01:00Z"))}, 
                    object::pair{std:("level"), as<std::shared_ptr<const>>(std:("debug"))}, 
                    object::pair{std:("message"), std:("Processing message")}
                } };
                (as<any>(agentsService))["get"]["mockResolvedValue"](mockLogs);
                auto result = std::async([=]() { agentsService->getAgentLogs(TEST_AGENT_ID); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/logs"), object{
                    object::pair{std:("params"), undefined}
                });
                expect(result)->toEqual(mockLogs);
            }
            );
            it(std:("should handle log parameters"), [=]() mutable
            {
                auto params = object{
                    object::pair{std:("limit"), 100}, 
                    object::pair{std:("level"), as<std::shared_ptr<const>>(std:("error"))}
                };
                (as<any>(agentsService))["get"]["mockResolvedValue"](array<any>());
                std::async([=]() { agentsService->getAgentLogs(TEST_AGENT_ID, params); });
                expect((as<any>(agentsService))["get"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/logs"), object{
                    object::pair{std:("params"), std:("params")}
                });
            }
            );
        }
        );
        describe(std:("deleteAgentLog"), [=]() mutable
        {
            it(std:("should delete agent log successfully"), [=]() mutable
            {
                auto mockResponse = object{
                    object::pair{std:("success"), true}
                };
                (as<any>(agentsService))["delete"]["mockResolvedValue"](mockResponse);
                auto result = std::async([=]() { agentsService->deleteAgentLog(TEST_AGENT_ID, TEST_LOG_ID); });
                expect((as<any>(agentsService))["delete"])->toHaveBeenCalledWith(std:("/api/agents/") + TEST_AGENT_ID + std:("/logs/") + TEST_LOG_ID + string_empty);
                expect(result)->toEqual(mockResponse);
            }
            );
        }
        );
        describe(std:("error handling"), [=]() mutable
        {
            it(std:("should handle network errors"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std:("Network error")));
                std::async([=]() { expect(agentsService->listAgents())->rejects->toThrow(std:("Network error")); });
            }
            );
            it(std:("should handle API errors"), [=]() mutable
            {
                (as<any>(agentsService))["post"]["mockRejectedValue"](std::make_shared<Error>(std:("API error")));
                std::async([=]() { expect(agentsService->createAgent(object{
                    object::pair{std:("name"), std:("test")}
                }))->rejects->toThrow(std:("API error")); });
            }
            );
            it(std:("should handle unauthorized errors"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std:("Unauthorized")));
                std::async([=]() { expect(agentsService->getAgent(TEST_AGENT_ID))->rejects->toThrow(std:("Unauthorized")); });
            }
            );
            it(std:("should handle rate limiting"), [=]() mutable
            {
                (as<any>(agentsService))["get"]["mockRejectedValue"](std::make_shared<Error>(std:("Rate limit exceeded")));
                std::async([=]() { expect(agentsService->listAgents())->rejects->toThrow(std:("Rate limit exceeded")); });
            }
            );
        }
        );
    }
    );
}

MAIN
