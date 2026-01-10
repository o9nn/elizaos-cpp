#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/providers/envStatus.test.h"

void Main(void)
{
    describe(std::string("envStatusProvider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = as<any>(object{
                object::pair{std::string("getService"), vi->fn()}, 
                object::pair{std::string("getSetting"), vi->fn()}, 
                object::pair{std::string("getWorld"), vi->fn()}
            });
            mockMessage = as<any>(object{
                object::pair{std::string("id"), std::string("test-message-id")}, 
                object::pair{std::string("entityId"), std::string("test-entity-id")}, 
                object::pair{std::string("roomId"), std::string("test-room-id")}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), std::string("check environment status")}
                }}
            });
            mockState = object{
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
        }
        );
        describe(std::string("provider properties"), [=]() mutable
        {
            it(std::string("should have correct name and description"), [=]() mutable
            {
                expect(envStatusProvider->name)->toBe(std::string("ENV_STATUS"));
                expect(envStatusProvider->description)->toContain(std::string("environment variables"));
            }
            );
        }
        );
        describe(std::string("get"), [=]() mutable
        {
            shared<any> mockEnvService;
            beforeEach([=]() mutable
            {
                mockEnvService = object{
                    object::pair{std::string("getEnvVarStatus"), vi->fn()}, 
                    object::pair{std::string("getMissingEnvVars"), vi->fn()}, 
                    object::pair{std::string("getGeneratableEnvVars"), vi->fn()}
                };
                (as<any>(mockRuntime->getService))["mockReturnValue"](mockEnvService);
            }
            );
            it(std::string("should return environment status when world is available"), [=]() mutable
            {
                auto mockWorld = object{
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), object{
                            object::pair{std::string("plugin1"), object{
                                object::pair{std::string("API_KEY"), object{
                                    object::pair{std::string("type"), std::string("api_key")}, 
                                    object::pair{std::string("required"), true}, 
                                    object::pair{std::string("description"), std::string("API key")}, 
                                    object::pair{std::string("canGenerate"), false}, 
                                    object::pair{std::string("status"), std::string("missing")}, 
                                    object::pair{std::string("attempts"), 0}, 
                                    object::pair{std::string("plugin"), std::string("plugin1")}
                                }}, 
                                object::pair{std::string("SECRET_KEY"), object{
                                    object::pair{std::string("type"), std::string("secret")}, 
                                    object::pair{std::string("required"), true}, 
                                    object::pair{std::string("description"), std::string("Secret key")}, 
                                    object::pair{std::string("canGenerate"), true}, 
                                    object::pair{std::string("status"), std::string("missing")}, 
                                    object::pair{std::string("attempts"), 0}, 
                                    object::pair{std::string("plugin"), std::string("plugin1")}
                                }}
                            }}
                        }}
                    }}
                };
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toContain(std::string("Environment Variables Status"));
                expect(result->text)->toContain(std::string("Plugin1 Plugin"));
                expect(result->text)->toContain(std::string("API_KEY"));
                expect(result->text)->toContain(std::string("SECRET_KEY"));
                expect(result->values->hasMissing)->toBe(true);
                expect(result->values->hasGeneratable)->toBe(true);
            }
            );
            it(std::string("should handle no world ID"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toBe(std::string("No world configuration found."));
                expect(result->values->hasMissing)->toBe(false);
            }
            );
            it(std::string("should handle errors gracefully"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockRejectedValue"](std::make_shared<Error>(std::string("Test error")));
                auto loggerSpy = vi->spyOn(logger, std::string("error"));
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toBe(std::string("Error retrieving environment variable status."));
                expect(result->values->hasMissing)->toBe(false);
                expect(loggerSpy)->toHaveBeenCalledWith(std::string("[EnvStatus] Error in environment status provider:"), std::make_shared<Error>(std::string("Test error")));
                loggerSpy->mockRestore();
            }
            );
            it(std::string("should handle no environment variables"), [=]() mutable
            {
                auto mockWorld = object{
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("envVars"), nullptr}
                    }}
                };
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std::string("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toBe(std::string("No environment variables configured yet."));
                expect(result->values->hasMissing)->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
