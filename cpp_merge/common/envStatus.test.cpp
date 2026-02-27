#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/providers/envStatus.test.h"

void Main(void)
{
    describe(std:("envStatusProvider"), [=]() mutable
    {
        shared<std::shared_ptr<IAgentRuntime>> mockRuntime;
        shared<std::shared_ptr<Memory>> mockMessage;
        shared<std::shared_ptr<State>> mockState;
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockRuntime = as<any>(object{
                object::pair{std:("getService"), vi->fn()}, 
                object::pair{std:("getSetting"), vi->fn()}, 
                object::pair{std:("getWorld"), vi->fn()}
            });
            mockMessage = as<any>(object{
                object::pair{std:("id"), std:("test-message-id")}, 
                object::pair{std:("entityId"), std:("test-entity-id")}, 
                object::pair{std:("roomId"), std:("test-room-id")}, 
                object::pair{std:("content"), object{
                    object::pair{std:("text"), std:("check environment status")}
                }}
            });
            mockState = object{
                object::pair{std:("values"), object{}}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
        }
        );
        describe(std:("provider properties"), [=]() mutable
        {
            it(std:("should have correct name and description"), [=]() mutable
            {
                expect(envStatusProvider->name)->toBe(std:("ENV_STATUS"));
                expect(envStatusProvider->description)->toContain(std:("environment variables"));
            }
            );
        }
        );
        describe(std:("get"), [=]() mutable
        {
            shared<any> mockEnvService;
            beforeEach([=]() mutable
            {
                mockEnvService = object{
                    object::pair{std:("getEnvVarStatus"), vi->fn()}, 
                    object::pair{std:("getMissingEnvVars"), vi->fn()}, 
                    object::pair{std:("getGeneratableEnvVars"), vi->fn()}
                };
                (as<any>(mockRuntime->getService))["mockReturnValue"](mockEnvService);
            }
            );
            it(std:("should return environment status when world is available"), [=]() mutable
            {
                auto mockWorld = object{
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), object{
                            object::pair{std:("plugin1"), object{
                                object::pair{std:("API_KEY"), object{
                                    object::pair{std:("type"), std:("api_key")}, 
                                    object::pair{std:("required"), true}, 
                                    object::pair{std:("description"), std:("API key")}, 
                                    object::pair{std:("canGenerate"), false}, 
                                    object::pair{std:("status"), std:("missing")}, 
                                    object::pair{std:("attempts"), 0}, 
                                    object::pair{std:("plugin"), std:("plugin1")}
                                }}, 
                                object::pair{std:("SECRET_KEY"), object{
                                    object::pair{std:("type"), std:("secret")}, 
                                    object::pair{std:("required"), true}, 
                                    object::pair{std:("description"), std:("Secret key")}, 
                                    object::pair{std:("canGenerate"), true}, 
                                    object::pair{std:("status"), std:("missing")}, 
                                    object::pair{std:("attempts"), 0}, 
                                    object::pair{std:("plugin"), std:("plugin1")}
                                }}
                            }}
                        }}
                    }}
                };
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toContain(std:("Environment Variables Status"));
                expect(result->text)->toContain(std:("Plugin1 Plugin"));
                expect(result->text)->toContain(std:("API_KEY"));
                expect(result->text)->toContain(std:("SECRET_KEY"));
                expect(result->values->hasMissing)->toBe(true);
                expect(result->values->hasGeneratable)->toBe(true);
            }
            );
            it(std:("should handle no world ID"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](nullptr);
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toBe(std:("No world configuration found."));
                expect(result->values->hasMissing)->toBe(false);
            }
            );
            it(std:("should handle errors gracefully"), [=]() mutable
            {
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockRejectedValue"](std::make_shared<Error>(std:("Test error")));
                auto loggerSpy = vi->spyOn(logger, std:("error"));
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toBe(std:("Error retrieving environment variable status."));
                expect(result->values->hasMissing)->toBe(false);
                expect(loggerSpy)->toHaveBeenCalledWith(std:("[EnvStatus] Error in environment status provider:"), std::make_shared<Error>(std:("Test error")));
                loggerSpy->mockRestore();
            }
            );
            it(std:("should handle no environment variables"), [=]() mutable
            {
                auto mockWorld = object{
                    object::pair{std:("metadata"), object{
                        object::pair{std:("envVars"), nullptr}
                    }}
                };
                (as<any>(mockRuntime->getSetting))["mockReturnValue"](std:("test-world-id"));
                (as<any>(mockRuntime->getWorld))["mockResolvedValue"](mockWorld);
                auto result = std::async([=]() { envStatusProvider->get(mockRuntime, mockMessage, mockState); });
                expect(result->text)->toBe(std:("No environment variables configured yet."));
                expect(result->values->hasMissing)->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
