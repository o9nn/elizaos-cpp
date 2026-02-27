#include "service.test.h"

std::function<any()> createMockRuntime = [=]() mutable
{
    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
        object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
        object::pair{std:("character"), as<std::shared_ptr<Character>>(object{
            object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-character-id"))}, 
            object::pair{std:("name"), std:("Test Character")}, 
            object::pair{std:("bio"), std:("Test bio")}, 
            object::pair{std:("settings"), object{
                object::pair{std:("secrets"), object{}}
            }}
        })}, 
        object::pair{std:("providers"), array<any>()}, 
        object::pair{std:("actions"), array<any>()}, 
        object::pair{std:("evaluators"), array<any>()}, 
        object::pair{std:("plugins"), as<array<std::shared_ptr<Plugin>>>(array<std::shared_ptr<Plugin>>())}, 
        object::pair{std:("services"), std::make_shared<Map>()}, 
        object::pair{std:("events"), std::make_shared<Map>()}, 
        object::pair{std:("fetch"), vi->fn()}, 
        object::pair{std:("routes"), array<any>()}, 
        object::pair{std:("registerPlugin"), vi->fn()}, 
        object::pair{std:("initialize"), vi->fn()}, 
        object::pair{std:("getConnection"), vi->fn()}, 
        object::pair{std:("getService"), vi->fn()}, 
        object::pair{std:("getAllServices"), vi->fn()}, 
        object::pair{std:("registerService"), vi->fn()}, 
        object::pair{std:("registerDatabaseAdapter"), vi->fn()}, 
        object::pair{std:("setSetting"), vi->fn()}, 
        object::pair{std:("getSetting"), vi->fn()}, 
        object::pair{std:("getConversationLength"), vi->fn()}, 
        object::pair{std:("processActions"), vi->fn()}, 
        object::pair{std:("evaluate"), vi->fn()}, 
        object::pair{std:("registerProvider"), vi->fn()}, 
        object::pair{std:("registerAction"), vi->fn()}, 
        object::pair{std:("registerEvaluator"), vi->fn()}, 
        object::pair{std:("ensureConnection"), vi->fn()}, 
        object::pair{std:("ensureParticipantInRoom"), vi->fn()}, 
        object::pair{std:("ensureWorldExists"), vi->fn()}, 
        object::pair{std:("ensureRoomExists"), vi->fn()}, 
        object::pair{std:("composeState"), vi->fn()}, 
        object::pair{std:("useModel"), vi->fn()}, 
        object::pair{std:("registerModel"), vi->fn()}, 
        object::pair{std:("getModel"), vi->fn()}, 
        object::pair{std:("registerEvent"), vi->fn()}, 
        object::pair{std:("getEvent"), vi->fn()}, 
        object::pair{std:("emitEvent"), vi->fn()}, 
        object::pair{std:("registerTaskWorker"), vi->fn()}, 
        object::pair{std:("getTaskWorker"), vi->fn()}, 
        object::pair{std:("stop"), vi->fn()}, 
        object::pair{std:("addEmbeddingToMemory"), vi->fn()}, 
        object::pair{std:("getEntityById"), vi->fn()}, 
        object::pair{std:("getRoom"), vi->fn()}, 
        object::pair{std:("createEntity"), vi->fn()}, 
        object::pair{std:("createRoom"), vi->fn()}, 
        object::pair{std:("addParticipant"), vi->fn()}, 
        object::pair{std:("getRooms"), vi->fn()}, 
        object::pair{std:("registerSendHandler"), vi->fn()}, 
        object::pair{std:("sendMessageToTarget"), vi->fn()}, 
        object::pair{std:("db"), object{}}, 
        object::pair{std:("init"), vi->fn()}, 
        object::pair{std:("close"), vi->fn()}, 
        object::pair{std:("getAgent"), vi->fn()}, 
        object::pair{std:("getAgents"), vi->fn()}, 
        object::pair{std:("createAgent"), vi->fn()}, 
        object::pair{std:("updateAgent"), vi->fn()}, 
        object::pair{std:("deleteAgent"), vi->fn()}, 
        object::pair{std:("ensureAgentExists"), vi->fn()}, 
        object::pair{std:("ensureEmbeddingDimension"), vi->fn()}, 
        object::pair{std:("getEntityByIds"), vi->fn()}, 
        object::pair{std:("getEntitiesForRoom"), vi->fn()}, 
        object::pair{std:("createEntities"), vi->fn()}, 
        object::pair{std:("updateEntity"), vi->fn()}, 
        object::pair{std:("getComponent"), vi->fn()}, 
        object::pair{std:("getComponents"), vi->fn()}, 
        object::pair{std:("createComponent"), vi->fn()}, 
        object::pair{std:("updateComponent"), vi->fn()}, 
        object::pair{std:("deleteComponent"), vi->fn()}, 
        object::pair{std:("getMemories"), vi->fn()}, 
        object::pair{std:("getMemoryById"), vi->fn()}, 
        object::pair{std:("getMemoriesByIds"), vi->fn()}, 
        object::pair{std:("getMemoriesByRoomIds"), vi->fn()}, 
        object::pair{std:("getMemoriesByServerId"), vi->fn()}, 
        object::pair{std:("getCachedEmbeddings"), vi->fn()}, 
        object::pair{std:("log"), vi->fn()}, 
        object::pair{std:("getLogs"), vi->fn()}, 
        object::pair{std:("deleteLog"), vi->fn()}, 
        object::pair{std:("searchMemories"), vi->fn()}, 
        object::pair{std:("createMemory"), vi->fn()}, 
        object::pair{std:("updateMemory"), vi->fn()}, 
        object::pair{std:("deleteMemory"), vi->fn()}, 
        object::pair{std:("deleteAllMemories"), vi->fn()}, 
        object::pair{std:("countMemories"), vi->fn()}, 
        object::pair{std:("createWorld"), vi->fn()}, 
        object::pair{std:("getWorld"), vi->fn()}, 
        object::pair{std:("removeWorld"), vi->fn()}, 
        object::pair{std:("getAllWorlds"), vi->fn()}, 
        object::pair{std:("updateWorld"), vi->fn()}, 
        object::pair{std:("getRoomsByIds"), vi->fn()}, 
        object::pair{std:("createRooms"), vi->fn()}, 
        object::pair{std:("deleteRoom"), vi->fn()}, 
        object::pair{std:("deleteRoomsByWorldId"), vi->fn()}, 
        object::pair{std:("updateRoom"), vi->fn()}, 
        object::pair{std:("getRoomsForParticipant"), vi->fn()}, 
        object::pair{std:("getRoomsForParticipants"), vi->fn()}, 
        object::pair{std:("getRoomsByWorld"), vi->fn()}, 
        object::pair{std:("removeParticipant"), vi->fn()}, 
        object::pair{std:("getParticipantsForEntity"), vi->fn()}, 
        object::pair{std:("getParticipantsForRoom"), vi->fn()}, 
        object::pair{std:("addParticipantsRoom"), vi->fn()}, 
        object::pair{std:("getParticipantUserState"), vi->fn()}, 
        object::pair{std:("setParticipantUserState"), vi->fn()}, 
        object::pair{std:("createRelationship"), vi->fn()}, 
        object::pair{std:("updateRelationship"), vi->fn()}, 
        object::pair{std:("getRelationship"), vi->fn()}, 
        object::pair{std:("getRelationships"), vi->fn()}, 
        object::pair{std:("getCache"), vi->fn()}, 
        object::pair{std:("setCache"), vi->fn()}, 
        object::pair{std:("deleteCache"), vi->fn()}, 
        object::pair{std:("createTask"), vi->fn()}, 
        object::pair{std:("getTasks"), vi->fn()}, 
        object::pair{std:("getTask"), vi->fn()}, 
        object::pair{std:("getTasksByName"), vi->fn()}, 
        object::pair{std:("updateTask"), vi->fn()}, 
        object::pair{std:("deleteTask"), vi->fn()}, 
        object::pair{std:("getMemoriesByWorldId"), vi->fn()}
    }));
    return mockRuntime;
};

void Main(void)
{
    vi->mock(std:("@elizaos/core"), [=](auto importOriginal) mutable
    {
        auto originalCore = as<Record<string, any>>((std::async([=]() { importOriginal(); })));
        return utils::assign(object{
            , 
            object::pair{std:("logger"), object{
                object::pair{std:("info"), vi->fn()}, 
                object::pair{std:("warn"), vi->fn()}, 
                object::pair{std:("error"), vi->fn()}, 
                object::pair{std:("debug"), vi->fn()}
            }}
        }, originalCore);
    }
    );
    describe(std:("EnvManagerService"), [=]() mutable
    {
        shared<std::shared_ptr<EnvManagerService>> envService;
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = createMockRuntime();
            envService = std::make_shared<EnvManagerService>(mockRuntime);
        }
        );
        it(std:("should be defined"), [=]() mutable
        {
            expect(envService)->toBeDefined();
        }
        );
        describe(std:("initialize"), [=]() mutable
        {
            it(std:("should call scanPluginRequirements and log initialization"), [=]() mutable
            {
                auto scanSpy = vi->spyOn(envService, std:("scanPluginRequirements"));
                std::async([=]() { envService->initialize(); });
                expect(scanSpy)->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std:("scanPluginRequirements"), [=]() mutable
        {
            it(std:("should log a warning if WORLD_ID is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto warnSpy = vi->spyOn(logger, std:("warn"));
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(warnSpy)->toHaveBeenCalledWith(std:("[EnvManager] No WORLD_ID found, cannot scan plugin requirements"));
            }
            );
            it(std:("should log a warning if world is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
                auto warnSpy = vi->spyOn(logger, std:("warn"));
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(warnSpy)->toHaveBeenCalledWith(std:("[EnvManager] World not found, cannot scan plugin requirements"));
            }
            );
            it(std:("should initialize metadata if it does not exist"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](as<std::shared_ptr<UUID>>(std:("test-world-id")));
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                    object::pair{std:("name"), std:("Test World")}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
                    object::pair{std:("serverId"), std:("test-server")}, 
                    object::pair{std:("metadata"), object{}}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(mockWorld->metadata->envVars)->toBeDefined();
                expect(mockWorld->metadata->generationScripts)->toBeDefined();
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
            }
            );
            it(std:("should scan character secrets and loaded plugins"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](as<std::shared_ptr<UUID>>(std:("test-world-id")));
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                    object::pair{std:("name"), std:("Test World")}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
                    object::pair{std:("serverId"), std:("test-server")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), object{}}, 
                        object::pair{std:("generationScripts"), object{}}
                    }}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                mockRuntime->character->settings = object{
                    object::pair{std:("secrets"), object{
                        object::pair{std:("CHARACTER_API_KEY"), std:("test-key")}
                    }}
                };
                mockRuntime->plugins = array<any>{ as<std::shared_ptr<Plugin>>(as<any>(object{
                    object::pair{std:("name"), std:("test-plugin")}, 
                    object::pair{std:("description"), std:("Test plugin")}, 
                    object::pair{std:("declaredEnvVars"), object{
                        object::pair{std:("PLUGIN_VAR"), object{
                            object::pair{std:("description"), std:("A test var")}, 
                            object::pair{std:("required"), true}
                        }}
                    }}
                })) };
                auto scanCharacterSecretsSpy = vi->spyOn(as<any>(envService), std:("scanCharacterSecrets"));
                auto scanLoadedPluginsSpy = vi->spyOn(as<any>(envService), std:("scanLoadedPlugins"));
                auto logSpy = vi->spyOn(logger, std:("info"));
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(scanCharacterSecretsSpy)->toHaveBeenCalled();
                expect(scanLoadedPluginsSpy)->toHaveBeenCalled();
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
                expect(logSpy)->toHaveBeenCalledWith(std:("[EnvManager] Plugin requirements scan completed"));
            }
            );
            it(std:("should handle existing envVars and generationScripts metadata"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](as<std::shared_ptr<UUID>>(std:("test-world-id")));
                auto existingEnvVars = object{
                    object::pair{std:("existing-plugin"), object{
                        object::pair{std:("EXISTING_VAR"), object{
                            object::pair{std:("type"), std:("config")}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Existing variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), std:("valid")}, 
                            object::pair{std:("attempts"), 1}, 
                            object::pair{std:("plugin"), std:("existing-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}
                    }}
                };
                auto existingGenerationScripts = object{
                    object::pair{std:("script1"), object{
                        object::pair{std:("variableName"), std:("OLD_SECRET")}, 
                        object::pair{std:("pluginName"), std:("old-plugin")}, 
                        object::pair{std:("script"), std:("std::cout << "old secret" << std::endl")}, 
                        object::pair{std:("dependencies"), array<any>()}, 
                        object::pair{std:("attempts"), 1}, 
                        object::pair{std:("status"), std:("success")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    }}
                };
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                    object::pair{std:("name"), std:("Test World")}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
                    object::pair{std:("serverId"), std:("test-server")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), existingEnvVars}, 
                        object::pair{std:("generationScripts"), existingGenerationScripts}
                    }}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(mockWorld->metadata->envVars)->toEqual(existingEnvVars);
                expect(mockWorld->metadata->generationScripts)->toEqual(existingGenerationScripts);
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
            }
            );
        }
        );
        describe(std:("getEnvVarsForPlugin"), [=]() mutable
        {
            it(std:("should return null if WORLD_ID is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto result = std::async([=]() { envService->getEnvVarsForPlugin(std:("test-plugin")); });
                expect(result)->toBeNull();
            }
            );
            it(std:("should return null if world is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
                auto result = std::async([=]() { envService->getEnvVarsForPlugin(std:("test-plugin")); });
                expect(result)->toBeNull();
            }
            );
            it(std:("should return plugin env vars if they exist"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std:("id"), as<std::shared_ptr<UUID>>(std:("test-world-id"))}, 
                    object::pair{std:("agentId"), as<std::shared_ptr<UUID>>(std:("test-agent-id"))}, 
                    object::pair{std:("serverId"), std:("test-server")}, 
                    object::pair{std:("name"), std:("Test World")}, 
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), object{
                            object::pair{std:("test-plugin"), object{
                                object::pair{std:("TEST_VAR"), object{
                                    object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                                    object::pair{std:("required"), true}, 
                                    object::pair{std:("description"), std:("Test variable")}, 
                                    object::pair{std:("canGenerate"), false}, 
                                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("valid"))}, 
                                    object::pair{std:("attempts"), 0}, 
                                    object::pair{std:("plugin"), std:("test-plugin")}, 
                                    object::pair{std:("createdAt"), Date->now()}
                                }}
                            }}
                        }}
                    }}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                auto result = std::async([=]() { envService->getEnvVarsForPlugin(std:("test-plugin")); });
                expect(result)->toEqual(const_(mockWorld->metadata->envVars)[std:("test-plugin")]);
            }
            );
        }
        );
        describe(std:("updateEnvVar"), [=]() mutable
        {
            it(std:("should return false if WORLD_ID is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto result = std::async([=]() { envService->updateEnvVar(std:("test-plugin"), std:("TEST_VAR"), object{
                    object::pair{std:("value"), std:("test")}
                }); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should return false if world is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
                auto result = std::async([=]() { envService->updateEnvVar(std:("test-plugin"), std:("TEST_VAR"), object{
                    object::pair{std:("value"), std:("test")}
                }); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should update environment variable and return true"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                auto mockWorld = object{
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), object{}}
                    }}
                };
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                (as<any>(mockRuntime->updateWorld))["mockResolvedValue"](undefined);
                auto result = std::async([=]() { envService->updateEnvVar(std:("test-plugin"), std:("TEST_VAR"), object{
                    object::pair{std:("value"), std:("test-value")}, 
                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("valid"))}
                }); });
                expect(result)->toBe(true);
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
                expect(process->env->TEST_VAR)->toBe(std:("test-value"));
            }
            );
            it(std:("should not update process.env if updateEnvVar fails"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                auto mockWorld = object{
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), object{}}
                    }}
                };
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                (as<any>(mockRuntime->updateWorld))["mockRejectedValue"](std::make_shared<Error>(std:("Update failed")));
                process->env->FAIL_VAR = std:("initial");
                auto result = std::async([=]() { envService->updateEnvVar(std:("fail-plugin"), std:("FAIL_VAR"), object{
                    object::pair{std:("value"), std:("new-value")}, 
                    object::pair{std:("status"), as<std::shared_ptr<const>>(std:("valid"))}
                }); });
                expect(result)->toBe(false);
                expect(process->env->FAIL_VAR)->toBe(std:("initial"));
                process->env.Delete("FAIL_VAR");
            }
            );
        }
        );
        describe(std:("hasMissingEnvVars"), [=]() mutable
        {
            it(std:("should return false if no env vars exist"), [=]() mutable
            {
                vi->spyOn(envService, std:("getAllEnvVars"))->mockResolvedValue(nullptr);
                auto result = std::async([=]() { envService->hasMissingEnvVars(); });
                expect(result)->toBe(false);
            }
            );
            it(std:("should return true if there are missing required env vars"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std:("test-plugin"), object{
                        object::pair{std:("MISSING_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Missing variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std:("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->hasMissingEnvVars(); });
                expect(result)->toBe(true);
            }
            );
            it(std:("should return false if all required env vars are present"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std:("test-plugin"), object{
                        object::pair{std:("PRESENT_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Present variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("valid"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}, 
                        object::pair{std:("OPTIONAL_MISSING_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), false}, 
                            object::pair{std:("description"), std:("Optional missing variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std:("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->hasMissingEnvVars(); });
                expect(result)->toBe(false);
            }
            );
        }
        );
        describe(std:("getMissingEnvVars"), [=]() mutable
        {
            it(std:("should return empty array if no env vars exist"), [=]() mutable
            {
                vi->spyOn(envService, std:("getAllEnvVars"))->mockResolvedValue(nullptr);
                auto result = std::async([=]() { envService->getMissingEnvVars(); });
                expect(result)->toEqual(array<any>());
            }
            );
            it(std:("should return missing required env vars"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std:("test-plugin"), object{
                        object::pair{std:("MISSING_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Missing variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}, 
                        object::pair{std:("PRESENT_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Present variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("valid"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std:("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->getMissingEnvVars(); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0]["varName"])->toBe(std:("MISSING_VAR"));
                expect(const_(result)[0]["plugin"])->toBe(std:("test-plugin"));
            }
            );
            it(std:("should only return required missing env vars"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std:("test-plugin"), object{
                        object::pair{std:("REQUIRED_MISSING_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Required missing variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}, 
                        object::pair{std:("OPTIONAL_MISSING_VAR"), object{
                            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                            object::pair{std:("required"), false}, 
                            object::pair{std:("description"), std:("Optional missing variable")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("test-plugin")}, 
                            object::pair{std:("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std:("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->getMissingEnvVars(); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0]["varName"])->toBe(std:("REQUIRED_MISSING_VAR"));
            }
            );
        }
        );
        describe(std:("getGeneratableEnvVars"), [=]() mutable
        {
            it(std:("should return only missing env vars that can be generated"), [=]() mutable
            {
                auto mockMissingVars = array<object>{ object{
                    object::pair{std:("plugin"), std:("test-plugin")}, 
                    object::pair{std:("varName"), std:("GENERATABLE_VAR")}, 
                    object::pair{std:("config"), object{
                        object::pair{std:("type"), as<std::shared_ptr<const>>(std:("api_key"))}, 
                        object::pair{std:("required"), true}, 
                        object::pair{std:("description"), std:("Generatable variable")}, 
                        object::pair{std:("canGenerate"), true}, 
                        object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                        object::pair{std:("attempts"), 0}, 
                        object::pair{std:("plugin"), std:("test-plugin")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    }}
                }, object{
                    object::pair{std:("plugin"), std:("test-plugin")}, 
                    object::pair{std:("varName"), std:("NON_GENERATABLE_VAR")}, 
                    object::pair{std:("config"), object{
                        object::pair{std:("type"), as<std::shared_ptr<const>>(std:("config"))}, 
                        object::pair{std:("required"), true}, 
                        object::pair{std:("description"), std:("Non-generatable variable")}, 
                        object::pair{std:("canGenerate"), false}, 
                        object::pair{std:("status"), as<std::shared_ptr<const>>(std:("missing"))}, 
                        object::pair{std:("attempts"), 0}, 
                        object::pair{std:("plugin"), std:("test-plugin")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    }}
                } };
                vi->spyOn(envService, std:("getMissingEnvVars"))->mockResolvedValue(mockMissingVars);
                auto result = std::async([=]() { envService->getGeneratableEnvVars(); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0]["varName"])->toBe(std:("GENERATABLE_VAR"));
                expect(const_(result)[0]["config"]->canGenerate)->toBe(true);
            }
            );
            it(std:("should return empty array if getMissingEnvVars returns empty"), [=]() mutable
            {
                vi->spyOn(envService, std:("getMissingEnvVars"))->mockResolvedValue(array<any>());
                auto result = std::async([=]() { envService->getGeneratableEnvVars(); });
                expect(result)->toEqual(array<any>());
            }
            );
        }
        );
    }
    );
}

MAIN
