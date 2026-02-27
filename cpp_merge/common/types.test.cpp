#include "types.test.h"

void Main(void)
{
    describe(std:("types"), [=]() mutable
    {
        describe(std:("EnvVarConfig"), [=]() mutable
        {
            it(std:("should allow valid EnvVarConfig objects"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("type"), std:("api_key")}, 
                    object::pair{std:("required"), true}, 
                    object::pair{std:("description"), std:("Test API key")}, 
                    object::pair{std:("canGenerate"), false}, 
                    object::pair{std:("status"), std:("missing")}, 
                    object::pair{std:("attempts"), 0}, 
                    object::pair{std:("plugin"), std:("test-plugin")}
                };
                expect(config->type)->toBe(std:("api_key"));
                expect(config->required)->toBe(true);
                expect(config->description)->toBe(std:("Test API key"));
                expect(config->canGenerate)->toBe(false);
                expect(config->status)->toBe(std:("missing"));
                expect(config->attempts)->toBe(0);
                expect(config->plugin)->toBe(std:("test-plugin"));
            }
            );
            it(std:("should allow optional fields"), [=]() mutable
            {
                auto config = object{
                    object::pair{std:("type"), std:("secret")}, 
                    object::pair{std:("required"), false}, 
                    object::pair{std:("description"), std:("Optional secret")}, 
                    object::pair{std:("canGenerate"), true}, 
                    object::pair{std:("status"), std:("valid")}, 
                    object::pair{std:("attempts"), 2}, 
                    object::pair{std:("plugin"), std:("test-plugin")}, 
                    object::pair{std:("value"), std:("secret-value")}, 
                    object::pair{std:("validatedAt"), Date->now()}, 
                    object::pair{std:("lastError"), std:("Previous validation failed")}, 
                    object::pair{std:("validationMethod"), std:("api_key:openai")}, 
                    object::pair{std:("createdAt"), Date->now()}
                };
                expect(config->value)->toBe(std:("secret-value"));
                expect(type_of(config->validatedAt))->toBe(std:("number"));
                expect(config->lastError)->toBe(std:("Previous validation failed"));
                expect(config->validationMethod)->toBe(std:("api_key:openai"));
                expect(type_of(config->createdAt))->toBe(std:("number"));
            }
            );
            it(std:("should support all valid types"), [=]() mutable
            {
                auto types = array<any>{ std:("api_key"), std:("private_key"), std:("public_key"), std:("secret"), std:("url"), std:("config"), std:("credential") };
                types->forEach([=](auto type) mutable
                {
                    auto config = object{
                        object::pair{std:("type"), std:("type")}, 
                        object::pair{std:("required"), true}, 
                        object::pair{std:("description"), std:("Test ") + type + string_empty}, 
                        object::pair{std:("canGenerate"), false}, 
                        object::pair{std:("status"), std:("missing")}, 
                        object::pair{std:("attempts"), 0}, 
                        object::pair{std:("plugin"), std:("test")}
                    };
                    expect(config->type)->toBe(type);
                }
                );
            }
            );
            it(std:("should support all valid statuses"), [=]() mutable
            {
                auto statuses = array<any>{ std:("missing"), std:("generating"), std:("validating"), std:("invalid"), std:("valid") };
                statuses->forEach([=](auto status) mutable
                {
                    auto config = object{
                        object::pair{std:("type"), std:("config")}, 
                        object::pair{std:("required"), true}, 
                        object::pair{std:("description"), std:("Test config")}, 
                        object::pair{std:("canGenerate"), false}, 
                        object::pair{std:("status"), std:("status")}, 
                        object::pair{std:("attempts"), 0}, 
                        object::pair{std:("plugin"), std:("test")}
                    };
                    expect(config->status)->toBe(status);
                }
                );
            }
            );
        }
        );
        describe(std:("EnvVarMetadata"), [=]() mutable
        {
            it(std:("should allow nested plugin configurations"), [=]() mutable
            {
                auto metadata = object{
                    object::pair{std:("plugin1"), object{
                        object::pair{std:("VAR1"), object{
                            object::pair{std:("type"), std:("api_key")}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Plugin 1 API key")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), std:("missing")}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("plugin1")}
                        }}, 
                        object::pair{std:("VAR2"), object{
                            object::pair{std:("type"), std:("secret")}, 
                            object::pair{std:("required"), false}, 
                            object::pair{std:("description"), std:("Plugin 1 secret")}, 
                            object::pair{std:("canGenerate"), true}, 
                            object::pair{std:("status"), std:("valid")}, 
                            object::pair{std:("attempts"), 1}, 
                            object::pair{std:("plugin"), std:("plugin1")}
                        }}
                    }}, 
                    object::pair{std:("plugin2"), object{
                        object::pair{std:("CONFIG_VAR"), object{
                            object::pair{std:("type"), std:("config")}, 
                            object::pair{std:("required"), true}, 
                            object::pair{std:("description"), std:("Plugin 2 config")}, 
                            object::pair{std:("canGenerate"), false}, 
                            object::pair{std:("status"), std:("valid")}, 
                            object::pair{std:("attempts"), 0}, 
                            object::pair{std:("plugin"), std:("plugin2")}
                        }}
                    }}
                };
                expect(Object->keys(metadata))->toEqual(array<string>{ std:("plugin1"), std:("plugin2") });
                expect(Object->keys(metadata->plugin1))->toEqual(array<string>{ std:("VAR1"), std:("VAR2") });
                expect(Object->keys(metadata->plugin2))->toEqual(array<string>{ std:("CONFIG_VAR") });
                expect(metadata->plugin1["VAR1"]->type)->toBe(std:("api_key"));
                expect(metadata->plugin2["CONFIG_VAR"]->type)->toBe(std:("config"));
            }
            );
        }
        );
        describe(std:("GenerationScript"), [=]() mutable
        {
            it(std:("should allow valid GenerationScript objects"), [=]() mutable
            {
                auto script = object{
                    object::pair{std:("variableName"), std:("SECRET_KEY")}, 
                    object::pair{std:("pluginName"), std:("test-plugin")}, 
                    object::pair{std:("script"), std:("std::cout << "generated value" << std::endl;")}, 
                    object::pair{std:("dependencies"), array<string>{ std:("crypto") }}, 
                    object::pair{std:("attempts"), 0}, 
                    object::pair{std:("status"), std:("pending")}, 
                    object::pair{std:("createdAt"), Date->now()}
                };
                expect(script->variableName)->toBe(std:("SECRET_KEY"));
                expect(script->pluginName)->toBe(std:("test-plugin"));
                expect(script->script)->toBe(std:("std::cout << "generated value" << std::endl;"));
                expect(script->dependencies)->toEqual(array<string>{ std:("crypto") });
                expect(script->attempts)->toBe(0);
                expect(script->status)->toBe(std:("pending"));
                expect(type_of(script->createdAt))->toBe(std:("number"));
            }
            );
            it(std:("should allow optional fields"), [=]() mutable
            {
                auto script = object{
                    object::pair{std:("variableName"), std:("API_KEY")}, 
                    object::pair{std:("pluginName"), std:("test-plugin")}, 
                    object::pair{std:("script"), std:("std::cout << "api key" << std::endl;")}, 
                    object::pair{std:("dependencies"), array<any>()}, 
                    object::pair{std:("attempts"), 2}, 
                    object::pair{std:("status"), std:("failed")}, 
                    object::pair{std:("createdAt"), Date->now()}, 
                    object::pair{std:("output"), std:("Generated output")}, 
                    object::pair{std:("error"), std:("Generation failed")}
                };
                expect(script->output)->toBe(std:("Generated output"));
                expect(script->error)->toBe(std:("Generation failed"));
            }
            );
            it(std:("should support all valid statuses"), [=]() mutable
            {
                auto statuses = array<any>{ std:("pending"), std:("running"), std:("success"), std:("failed") };
                statuses->forEach([=](auto status) mutable
                {
                    auto script = object{
                        object::pair{std:("variableName"), std:("TEST_VAR")}, 
                        object::pair{std:("pluginName"), std:("test")}, 
                        object::pair{std:("script"), std:("std::cout << "test" << std::endl;")}, 
                        object::pair{std:("dependencies"), array<any>()}, 
                        object::pair{std:("attempts"), 0}, 
                        object::pair{std:("status"), std:("status")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    };
                    expect(script->status)->toBe(status);
                }
                );
            }
            );
        }
        );
        describe(std:("GenerationScriptMetadata"), [=]() mutable
        {
            it(std:("should allow script configurations by script ID"), [=]() mutable
            {
                auto metadata = object{
                    object::pair{std:("script1"), object{
                        object::pair{std:("variableName"), std:("SECRET_KEY")}, 
                        object::pair{std:("pluginName"), std:("plugin1")}, 
                        object::pair{std:("script"), std:("crypto.randomBytes(32).toString("hex")")}, 
                        object::pair{std:("dependencies"), array<any>()}, 
                        object::pair{std:("attempts"), 0}, 
                        object::pair{std:("status"), std:("pending")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    }}, 
                    object::pair{std:("script2"), object{
                        object::pair{std:("variableName"), std:("PRIVATE_KEY")}, 
                        object::pair{std:("pluginName"), std:("plugin2")}, 
                        object::pair{std:("script"), std:("generateKeyPair()")}, 
                        object::pair{std:("dependencies"), array<string>{ std:("crypto") }}, 
                        object::pair{std:("attempts"), 1}, 
                        object::pair{std:("status"), std:("success")}, 
                        object::pair{std:("createdAt"), Date->now()}
                    }}
                };
                expect(Object->keys(metadata))->toEqual(array<string>{ std:("script1"), std:("script2") });
                expect(metadata->script1->variableName)->toBe(std:("SECRET_KEY"));
                expect(metadata->script2->status)->toBe(std:("success"));
            }
            );
        }
        );
        describe(std:("ValidationResult"), [=]() mutable
        {
            it(std:("should allow valid ValidationResult objects"), [=]() mutable
            {
                auto validResult = object{
                    object::pair{std:("isValid"), true}, 
                    object::pair{std:("details"), std:("Validation passed successfully")}
                };
                expect(validResult->isValid)->toBe(true);
                expect(validResult->details)->toBe(std:("Validation passed successfully"));
                expect(validResult->error)->toBeUndefined();
            }
            );
            it(std:("should allow invalid ValidationResult objects"), [=]() mutable
            {
                auto invalidResult = object{
                    object::pair{std:("isValid"), false}, 
                    object::pair{std:("error"), std:("Validation failed")}, 
                    object::pair{std:("details"), std:("API key format is incorrect")}
                };
                expect(invalidResult->isValid)->toBe(false);
                expect(invalidResult->error)->toBe(std:("Validation failed"));
                expect(invalidResult->details)->toBe(std:("API key format is incorrect"));
            }
            );
            it(std:("should allow minimal ValidationResult objects"), [=]() mutable
            {
                auto minimalValid = object{
                    object::pair{std:("isValid"), true}
                };
                auto minimalInvalid = object{
                    object::pair{std:("isValid"), false}
                };
                expect(minimalValid->isValid)->toBe(true);
                expect(minimalInvalid->isValid)->toBe(false);
            }
            );
        }
        );
        describe(std:("EnvVarUpdate"), [=]() mutable
        {
            it(std:("should allow valid EnvVarUpdate objects"), [=]() mutable
            {
                auto update = object{
                    object::pair{std:("pluginName"), std:("test-plugin")}, 
                    object::pair{std:("variableName"), std:("API_KEY")}, 
                    object::pair{std:("value"), std:("sk-test123")}
                };
                expect(update->pluginName)->toBe(std:("test-plugin"));
                expect(update->variableName)->toBe(std:("API_KEY"));
                expect(update->value)->toBe(std:("sk-test123"));
            }
            );
        }
        );
        describe(std:("Type compatibility"), [=]() mutable
        {
            it(std:("should allow empty metadata objects"), [=]() mutable
            {
                auto emptyEnvVars = object{};
                auto emptyScripts = object{};
                expect(Object->keys(emptyEnvVars))->toEqual(array<any>());
                expect(Object->keys(emptyScripts))->toEqual(array<any>());
            }
            );
            it(std:("should work with all credential types"), [=]() mutable
            {
                auto credentialConfig = object{
                    object::pair{std:("type"), std:("credential")}, 
                    object::pair{std:("required"), true}, 
                    object::pair{std:("description"), std:("Database credentials")}, 
                    object::pair{std:("canGenerate"), false}, 
                    object::pair{std:("status"), std:("missing")}, 
                    object::pair{std:("attempts"), 0}, 
                    object::pair{std:("plugin"), std:("database-plugin")}
                };
                expect(credentialConfig->type)->toBe(std:("credential"));
            }
            );
        }
        );
    }
    );
}

MAIN
