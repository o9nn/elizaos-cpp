#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/service.test.h"

std::function<any()> createMockRuntime = [=]() mutable
{
    auto mockRuntime = as<std::shared_ptr<IAgentRuntime>>(as<any>(object{
        object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
        object::pair{std::string("character"), as<std::shared_ptr<Character>>(object{
            object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-character-id"))}, 
            object::pair{std::string("name"), std::string("Test Character")}, 
            object::pair{std::string("bio"), std::string("Test bio")}, 
            object::pair{std::string("settings"), object{
                object::pair{std::string("secrets"), object{}}
            }}
        })}, 
        object::pair{std::string("providers"), array<any>()}, 
        object::pair{std::string("actions"), array<any>()}, 
        object::pair{std::string("evaluators"), array<any>()}, 
        object::pair{std::string("plugins"), as<array<std::shared_ptr<Plugin>>>(array<std::shared_ptr<Plugin>>())}, 
        object::pair{std::string("services"), std::make_shared<Map>()}, 
        object::pair{std::string("events"), std::make_shared<Map>()}, 
        object::pair{std::string("fetch"), vi->fn()}, 
        object::pair{std::string("routes"), array<any>()}, 
        object::pair{std::string("registerPlugin"), vi->fn()}, 
        object::pair{std::string("initialize"), vi->fn()}, 
        object::pair{std::string("getConnection"), vi->fn()}, 
        object::pair{std::string("getService"), vi->fn()}, 
        object::pair{std::string("getAllServices"), vi->fn()}, 
        object::pair{std::string("registerService"), vi->fn()}, 
        object::pair{std::string("registerDatabaseAdapter"), vi->fn()}, 
        object::pair{std::string("setSetting"), vi->fn()}, 
        object::pair{std::string("getSetting"), vi->fn()}, 
        object::pair{std::string("getConversationLength"), vi->fn()}, 
        object::pair{std::string("processActions"), vi->fn()}, 
        object::pair{std::string("evaluate"), vi->fn()}, 
        object::pair{std::string("registerProvider"), vi->fn()}, 
        object::pair{std::string("registerAction"), vi->fn()}, 
        object::pair{std::string("registerEvaluator"), vi->fn()}, 
        object::pair{std::string("ensureConnection"), vi->fn()}, 
        object::pair{std::string("ensureParticipantInRoom"), vi->fn()}, 
        object::pair{std::string("ensureWorldExists"), vi->fn()}, 
        object::pair{std::string("ensureRoomExists"), vi->fn()}, 
        object::pair{std::string("composeState"), vi->fn()}, 
        object::pair{std::string("useModel"), vi->fn()}, 
        object::pair{std::string("registerModel"), vi->fn()}, 
        object::pair{std::string("getModel"), vi->fn()}, 
        object::pair{std::string("registerEvent"), vi->fn()}, 
        object::pair{std::string("getEvent"), vi->fn()}, 
        object::pair{std::string("emitEvent"), vi->fn()}, 
        object::pair{std::string("registerTaskWorker"), vi->fn()}, 
        object::pair{std::string("getTaskWorker"), vi->fn()}, 
        object::pair{std::string("stop"), vi->fn()}, 
        object::pair{std::string("addEmbeddingToMemory"), vi->fn()}, 
        object::pair{std::string("getEntityById"), vi->fn()}, 
        object::pair{std::string("getRoom"), vi->fn()}, 
        object::pair{std::string("createEntity"), vi->fn()}, 
        object::pair{std::string("createRoom"), vi->fn()}, 
        object::pair{std::string("addParticipant"), vi->fn()}, 
        object::pair{std::string("getRooms"), vi->fn()}, 
        object::pair{std::string("registerSendHandler"), vi->fn()}, 
        object::pair{std::string("sendMessageToTarget"), vi->fn()}, 
        object::pair{std::string("db"), object{}}, 
        object::pair{std::string("init"), vi->fn()}, 
        object::pair{std::string("close"), vi->fn()}, 
        object::pair{std::string("getAgent"), vi->fn()}, 
        object::pair{std::string("getAgents"), vi->fn()}, 
        object::pair{std::string("createAgent"), vi->fn()}, 
        object::pair{std::string("updateAgent"), vi->fn()}, 
        object::pair{std::string("deleteAgent"), vi->fn()}, 
        object::pair{std::string("ensureAgentExists"), vi->fn()}, 
        object::pair{std::string("ensureEmbeddingDimension"), vi->fn()}, 
        object::pair{std::string("getEntityByIds"), vi->fn()}, 
        object::pair{std::string("getEntitiesForRoom"), vi->fn()}, 
        object::pair{std::string("createEntities"), vi->fn()}, 
        object::pair{std::string("updateEntity"), vi->fn()}, 
        object::pair{std::string("getComponent"), vi->fn()}, 
        object::pair{std::string("getComponents"), vi->fn()}, 
        object::pair{std::string("createComponent"), vi->fn()}, 
        object::pair{std::string("updateComponent"), vi->fn()}, 
        object::pair{std::string("deleteComponent"), vi->fn()}, 
        object::pair{std::string("getMemories"), vi->fn()}, 
        object::pair{std::string("getMemoryById"), vi->fn()}, 
        object::pair{std::string("getMemoriesByIds"), vi->fn()}, 
        object::pair{std::string("getMemoriesByRoomIds"), vi->fn()}, 
        object::pair{std::string("getMemoriesByServerId"), vi->fn()}, 
        object::pair{std::string("getCachedEmbeddings"), vi->fn()}, 
        object::pair{std::string("log"), vi->fn()}, 
        object::pair{std::string("getLogs"), vi->fn()}, 
        object::pair{std::string("deleteLog"), vi->fn()}, 
        object::pair{std::string("searchMemories"), vi->fn()}, 
        object::pair{std::string("createMemory"), vi->fn()}, 
        object::pair{std::string("updateMemory"), vi->fn()}, 
        object::pair{std::string("deleteMemory"), vi->fn()}, 
        object::pair{std::string("deleteAllMemories"), vi->fn()}, 
        object::pair{std::string("countMemories"), vi->fn()}, 
        object::pair{std::string("createWorld"), vi->fn()}, 
        object::pair{std::string("getWorld"), vi->fn()}, 
        object::pair{std::string("removeWorld"), vi->fn()}, 
        object::pair{std::string("getAllWorlds"), vi->fn()}, 
        object::pair{std::string("updateWorld"), vi->fn()}, 
        object::pair{std::string("getRoomsByIds"), vi->fn()}, 
        object::pair{std::string("createRooms"), vi->fn()}, 
        object::pair{std::string("deleteRoom"), vi->fn()}, 
        object::pair{std::string("deleteRoomsByWorldId"), vi->fn()}, 
        object::pair{std::string("updateRoom"), vi->fn()}, 
        object::pair{std::string("getRoomsForParticipant"), vi->fn()}, 
        object::pair{std::string("getRoomsForParticipants"), vi->fn()}, 
        object::pair{std::string("getRoomsByWorld"), vi->fn()}, 
        object::pair{std::string("removeParticipant"), vi->fn()}, 
        object::pair{std::string("getParticipantsForEntity"), vi->fn()}, 
        object::pair{std::string("getParticipantsForRoom"), vi->fn()}, 
        object::pair{std::string("addParticipantsRoom"), vi->fn()}, 
        object::pair{std::string("getParticipantUserState"), vi->fn()}, 
        object::pair{std::string("setParticipantUserState"), vi->fn()}, 
        object::pair{std::string("createRelationship"), vi->fn()}, 
        object::pair{std::string("updateRelationship"), vi->fn()}, 
        object::pair{std::string("getRelationship"), vi->fn()}, 
        object::pair{std::string("getRelationships"), vi->fn()}, 
        object::pair{std::string("getCache"), vi->fn()}, 
        object::pair{std::string("setCache"), vi->fn()}, 
        object::pair{std::string("deleteCache"), vi->fn()}, 
        object::pair{std::string("createTask"), vi->fn()}, 
        object::pair{std::string("getTasks"), vi->fn()}, 
        object::pair{std::string("getTask"), vi->fn()}, 
        object::pair{std::string("getTasksByName"), vi->fn()}, 
        object::pair{std::string("updateTask"), vi->fn()}, 
        object::pair{std::string("deleteTask"), vi->fn()}, 
        object::pair{std::string("getMemoriesByWorldId"), vi->fn()}
    }));
    return mockRuntime;
};

void Main(void)
{
    vi->mock(std::string("@elizaos/core"), [=](auto importOriginal) mutable
    {
        auto originalCore = as<Record<string, any>>((std::async([=]() { importOriginal(); })));
        return utils::assign(object{
            , 
            object::pair{std::string("logger"), object{
                object::pair{std::string("info"), vi->fn()}, 
                object::pair{std::string("warn"), vi->fn()}, 
                object::pair{std::string("error"), vi->fn()}, 
                object::pair{std::string("debug"), vi->fn()}
            }}
        }, originalCore);
    }
    );
    describe(std::string("EnvManagerService"), [=]() mutable
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
        it(std::string("should be defined"), [=]() mutable
        {
            expect(envService)->toBeDefined();
        }
        );
        describe(std::string("initialize"), [=]() mutable
        {
            it(std::string("should call scanPluginRequirements and log initialization"), [=]() mutable
            {
                auto scanSpy = vi->spyOn(envService, std::string("scanPluginRequirements"));
                std::async([=]() { envService->initialize(); });
                expect(scanSpy)->toHaveBeenCalled();
            }
            );
        }
        );
        describe(std::string("scanPluginRequirements"), [=]() mutable
        {
            it(std::string("should log a warning if WORLD_ID is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto warnSpy = vi->spyOn(logger, std::string("warn"));
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(warnSpy)->toHaveBeenCalledWith(std::string("[EnvManager] No WORLD_ID found, cannot scan plugin requirements"));
            }
            );
            it(std::string("should log a warning if world is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
                auto warnSpy = vi->spyOn(logger, std::string("warn"));
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(warnSpy)->toHaveBeenCalledWith(std::string("[EnvManager] World not found, cannot scan plugin requirements"));
            }
            );
            it(std::string("should initialize metadata if it does not exist"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](as<std::shared_ptr<UUID>>(std::string("test-world-id")));
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                    object::pair{std::string("name"), std::string("Test World")}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
                    object::pair{std::string("serverId"), std::string("test-server")}, 
                    object::pair{std::string("metadata"), object{}}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(mockWorld->metadata->envVars)->toBeDefined();
                expect(mockWorld->metadata->generationScripts)->toBeDefined();
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
            }
            );
            it(std::string("should scan character secrets and loaded plugins"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](as<std::shared_ptr<UUID>>(std::string("test-world-id")));
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                    object::pair{std::string("name"), std::string("Test World")}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
                    object::pair{std::string("serverId"), std::string("test-server")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), object{}}, 
                        object::pair{std::string("generationScripts"), object{}}
                    }}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                mockRuntime->character->settings = object{
                    object::pair{std::string("secrets"), object{
                        object::pair{std::string("CHARACTER_API_KEY"), std::string("test-key")}
                    }}
                };
                mockRuntime->plugins = array<any>{ as<std::shared_ptr<Plugin>>(as<any>(object{
                    object::pair{std::string("name"), std::string("test-plugin")}, 
                    object::pair{std::string("description"), std::string("Test plugin")}, 
                    object::pair{std::string("declaredEnvVars"), object{
                        object::pair{std::string("PLUGIN_VAR"), object{
                            object::pair{std::string("description"), std::string("A test var")}, 
                            object::pair{std::string("required"), true}
                        }}
                    }}
                })) };
                auto scanCharacterSecretsSpy = vi->spyOn(as<any>(envService), std::string("scanCharacterSecrets"));
                auto scanLoadedPluginsSpy = vi->spyOn(as<any>(envService), std::string("scanLoadedPlugins"));
                auto logSpy = vi->spyOn(logger, std::string("info"));
                std::async([=]() { envService->scanPluginRequirements(); });
                expect(scanCharacterSecretsSpy)->toHaveBeenCalled();
                expect(scanLoadedPluginsSpy)->toHaveBeenCalled();
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
                expect(logSpy)->toHaveBeenCalledWith(std::string("[EnvManager] Plugin requirements scan completed"));
            }
            );
            it(std::string("should handle existing envVars and generationScripts metadata"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](as<std::shared_ptr<UUID>>(std::string("test-world-id")));
                auto existingEnvVars = object{
                    object::pair{std::string("existing-plugin"), object{
                        object::pair{std::string("EXISTING_VAR"), object{
                            object::pair{std::string("type"), std::string("config")}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Existing variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), std::string("valid")}, 
                            object::pair{std::string("attempts"), 1}, 
                            object::pair{std::string("plugin"), std::string("existing-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}
                    }}
                };
                auto existingGenerationScripts = object{
                    object::pair{std::string("script1"), object{
                        object::pair{std::string("variableName"), std::string("OLD_SECRET")}, 
                        object::pair{std::string("pluginName"), std::string("old-plugin")}, 
                        object::pair{std::string("script"), std::string("console.log("old secret")")}, 
                        object::pair{std::string("dependencies"), array<any>()}, 
                        object::pair{std::string("attempts"), 1}, 
                        object::pair{std::string("status"), std::string("success")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    }}
                };
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                    object::pair{std::string("name"), std::string("Test World")}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
                    object::pair{std::string("serverId"), std::string("test-server")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), existingEnvVars}, 
                        object::pair{std::string("generationScripts"), existingGenerationScripts}
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
        describe(std::string("getEnvVarsForPlugin"), [=]() mutable
        {
            it(std::string("should return null if WORLD_ID is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto result = std::async([=]() { envService->getEnvVarsForPlugin(std::string("test-plugin")); });
                expect(result)->toBeNull();
            }
            );
            it(std::string("should return null if world is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
                auto result = std::async([=]() { envService->getEnvVarsForPlugin(std::string("test-plugin")); });
                expect(result)->toBeNull();
            }
            );
            it(std::string("should return plugin env vars if they exist"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                auto mockWorld = as<std::shared_ptr<World>>(object{
                    object::pair{std::string("id"), as<std::shared_ptr<UUID>>(std::string("test-world-id"))}, 
                    object::pair{std::string("agentId"), as<std::shared_ptr<UUID>>(std::string("test-agent-id"))}, 
                    object::pair{std::string("serverId"), std::string("test-server")}, 
                    object::pair{std::string("name"), std::string("Test World")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), object{
                            object::pair{std::string("test-plugin"), object{
                                object::pair{std::string("TEST_VAR"), object{
                                    object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                                    object::pair{std::string("required"), true}, 
                                    object::pair{std::string("description"), std::string("Test variable")}, 
                                    object::pair{std::string("canGenerate"), false}, 
                                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("valid"))}, 
                                    object::pair{std::string("attempts"), 0}, 
                                    object::pair{std::string("plugin"), std::string("test-plugin")}, 
                                    object::pair{std::string("createdAt"), Date->now()}
                                }}
                            }}
                        }}
                    }}
                });
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                auto result = std::async([=]() { envService->getEnvVarsForPlugin(std::string("test-plugin")); });
                expect(result)->toEqual(const_(mockWorld->metadata->envVars)[std::string("test-plugin")]);
            }
            );
        }
        );
        describe(std::string("updateEnvVar"), [=]() mutable
        {
            it(std::string("should return false if WORLD_ID is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto result = std::async([=]() { envService->updateEnvVar(std::string("test-plugin"), std::string("TEST_VAR"), object{
                    object::pair{std::string("value"), std::string("test")}
                }); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should return false if world is not found"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](nullptr);
                auto result = std::async([=]() { envService->updateEnvVar(std::string("test-plugin"), std::string("TEST_VAR"), object{
                    object::pair{std::string("value"), std::string("test")}
                }); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should update environment variable and return true"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                auto mockWorld = object{
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), object{}}
                    }}
                };
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                (as<any>(mockRuntime->updateWorld))["mockResolvedValue"](undefined);
                auto result = std::async([=]() { envService->updateEnvVar(std::string("test-plugin"), std::string("TEST_VAR"), object{
                    object::pair{std::string("value"), std::string("test-value")}, 
                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("valid"))}
                }); });
                expect(result)->toBe(true);
                expect(mockRuntime->updateWorld)->toHaveBeenCalledWith(mockWorld);
                expect(process->env->TEST_VAR)->toBe(std::string("test-value"));
            }
            );
            it(std::string("should not update process.env if updateEnvVar fails"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                auto mockWorld = object{
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), object{}}
                    }}
                };
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                (as<any>(mockRuntime->updateWorld))["mockRejectedValue"](std::make_shared<Error>(std::string("Update failed")));
                process->env->FAIL_VAR = std::string("initial");
                auto result = std::async([=]() { envService->updateEnvVar(std::string("fail-plugin"), std::string("FAIL_VAR"), object{
                    object::pair{std::string("value"), std::string("new-value")}, 
                    object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("valid"))}
                }); });
                expect(result)->toBe(false);
                expect(process->env->FAIL_VAR)->toBe(std::string("initial"));
                process->env.Delete("FAIL_VAR");
            }
            );
        }
        );
        describe(std::string("hasMissingEnvVars"), [=]() mutable
        {
            it(std::string("should return false if no env vars exist"), [=]() mutable
            {
                vi->spyOn(envService, std::string("getAllEnvVars"))->mockResolvedValue(nullptr);
                auto result = std::async([=]() { envService->hasMissingEnvVars(); });
                expect(result)->toBe(false);
            }
            );
            it(std::string("should return true if there are missing required env vars"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std::string("test-plugin"), object{
                        object::pair{std::string("MISSING_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Missing variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std::string("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->hasMissingEnvVars(); });
                expect(result)->toBe(true);
            }
            );
            it(std::string("should return false if all required env vars are present"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std::string("test-plugin"), object{
                        object::pair{std::string("PRESENT_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Present variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("valid"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}, 
                        object::pair{std::string("OPTIONAL_MISSING_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), false}, 
                            object::pair{std::string("description"), std::string("Optional missing variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std::string("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->hasMissingEnvVars(); });
                expect(result)->toBe(false);
            }
            );
        }
        );
        describe(std::string("getMissingEnvVars"), [=]() mutable
        {
            it(std::string("should return empty array if no env vars exist"), [=]() mutable
            {
                vi->spyOn(envService, std::string("getAllEnvVars"))->mockResolvedValue(nullptr);
                auto result = std::async([=]() { envService->getMissingEnvVars(); });
                expect(result)->toEqual(array<any>());
            }
            );
            it(std::string("should return missing required env vars"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std::string("test-plugin"), object{
                        object::pair{std::string("MISSING_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Missing variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}, 
                        object::pair{std::string("PRESENT_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Present variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("valid"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std::string("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->getMissingEnvVars(); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0]["varName"])->toBe(std::string("MISSING_VAR"));
                expect(const_(result)[0]["plugin"])->toBe(std::string("test-plugin"));
            }
            );
            it(std::string("should only return required missing env vars"), [=]() mutable
            {
                auto mockEnvVars = object{
                    object::pair{std::string("test-plugin"), object{
                        object::pair{std::string("REQUIRED_MISSING_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Required missing variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}, 
                        object::pair{std::string("OPTIONAL_MISSING_VAR"), object{
                            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                            object::pair{std::string("required"), false}, 
                            object::pair{std::string("description"), std::string("Optional missing variable")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("test-plugin")}, 
                            object::pair{std::string("createdAt"), Date->now()}
                        }}
                    }}
                };
                vi->spyOn(envService, std::string("getAllEnvVars"))->mockResolvedValue(mockEnvVars);
                auto result = std::async([=]() { envService->getMissingEnvVars(); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0]["varName"])->toBe(std::string("REQUIRED_MISSING_VAR"));
            }
            );
        }
        );
        describe(std::string("getGeneratableEnvVars"), [=]() mutable
        {
            it(std::string("should return only missing env vars that can be generated"), [=]() mutable
            {
                auto mockMissingVars = array<object>{ object{
                    object::pair{std::string("plugin"), std::string("test-plugin")}, 
                    object::pair{std::string("varName"), std::string("GENERATABLE_VAR")}, 
                    object::pair{std::string("config"), object{
                        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("api_key"))}, 
                        object::pair{std::string("required"), true}, 
                        object::pair{std::string("description"), std::string("Generatable variable")}, 
                        object::pair{std::string("canGenerate"), true}, 
                        object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                        object::pair{std::string("attempts"), 0}, 
                        object::pair{std::string("plugin"), std::string("test-plugin")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    }}
                }, object{
                    object::pair{std::string("plugin"), std::string("test-plugin")}, 
                    object::pair{std::string("varName"), std::string("NON_GENERATABLE_VAR")}, 
                    object::pair{std::string("config"), object{
                        object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("config"))}, 
                        object::pair{std::string("required"), true}, 
                        object::pair{std::string("description"), std::string("Non-generatable variable")}, 
                        object::pair{std::string("canGenerate"), false}, 
                        object::pair{std::string("status"), as<std::shared_ptr<const>>(std::string("missing"))}, 
                        object::pair{std::string("attempts"), 0}, 
                        object::pair{std::string("plugin"), std::string("test-plugin")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    }}
                } };
                vi->spyOn(envService, std::string("getMissingEnvVars"))->mockResolvedValue(mockMissingVars);
                auto result = std::async([=]() { envService->getGeneratableEnvVars(); });
                expect(result)->toHaveLength(1);
                expect(const_(result)[0]["varName"])->toBe(std::string("GENERATABLE_VAR"));
                expect(const_(result)[0]["config"]->canGenerate)->toBe(true);
            }
            );
            it(std::string("should return empty array if getMissingEnvVars returns empty"), [=]() mutable
            {
                vi->spyOn(envService, std::string("getMissingEnvVars"))->mockResolvedValue(array<any>());
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
