#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/types.test.h"

void Main(void)
{
    describe(std::string("types"), [=]() mutable
    {
        describe(std::string("EnvVarConfig"), [=]() mutable
        {
            it(std::string("should allow valid EnvVarConfig objects"), [=]() mutable
            {
                auto config = object{
                    object::pair{std::string("type"), std::string("api_key")}, 
                    object::pair{std::string("required"), true}, 
                    object::pair{std::string("description"), std::string("Test API key")}, 
                    object::pair{std::string("canGenerate"), false}, 
                    object::pair{std::string("status"), std::string("missing")}, 
                    object::pair{std::string("attempts"), 0}, 
                    object::pair{std::string("plugin"), std::string("test-plugin")}
                };
                expect(config->type)->toBe(std::string("api_key"));
                expect(config->required)->toBe(true);
                expect(config->description)->toBe(std::string("Test API key"));
                expect(config->canGenerate)->toBe(false);
                expect(config->status)->toBe(std::string("missing"));
                expect(config->attempts)->toBe(0);
                expect(config->plugin)->toBe(std::string("test-plugin"));
            }
            );
            it(std::string("should allow optional fields"), [=]() mutable
            {
                auto config = object{
                    object::pair{std::string("type"), std::string("secret")}, 
                    object::pair{std::string("required"), false}, 
                    object::pair{std::string("description"), std::string("Optional secret")}, 
                    object::pair{std::string("canGenerate"), true}, 
                    object::pair{std::string("status"), std::string("valid")}, 
                    object::pair{std::string("attempts"), 2}, 
                    object::pair{std::string("plugin"), std::string("test-plugin")}, 
                    object::pair{std::string("value"), std::string("secret-value")}, 
                    object::pair{std::string("validatedAt"), Date->now()}, 
                    object::pair{std::string("lastError"), std::string("Previous validation failed")}, 
                    object::pair{std::string("validationMethod"), std::string("api_key:openai")}, 
                    object::pair{std::string("createdAt"), Date->now()}
                };
                expect(config->value)->toBe(std::string("secret-value"));
                expect(type_of(config->validatedAt))->toBe(std::string("number"));
                expect(config->lastError)->toBe(std::string("Previous validation failed"));
                expect(config->validationMethod)->toBe(std::string("api_key:openai"));
                expect(type_of(config->createdAt))->toBe(std::string("number"));
            }
            );
            it(std::string("should support all valid types"), [=]() mutable
            {
                auto types = array<any>{ std::string("api_key"), std::string("private_key"), std::string("public_key"), std::string("secret"), std::string("url"), std::string("config"), std::string("credential") };
                types->forEach([=](auto type) mutable
                {
                    auto config = object{
                        object::pair{std::string("type"), std::string("type")}, 
                        object::pair{std::string("required"), true}, 
                        object::pair{std::string("description"), std::string("Test ") + type + string_empty}, 
                        object::pair{std::string("canGenerate"), false}, 
                        object::pair{std::string("status"), std::string("missing")}, 
                        object::pair{std::string("attempts"), 0}, 
                        object::pair{std::string("plugin"), std::string("test")}
                    };
                    expect(config->type)->toBe(type);
                }
                );
            }
            );
            it(std::string("should support all valid statuses"), [=]() mutable
            {
                auto statuses = array<any>{ std::string("missing"), std::string("generating"), std::string("validating"), std::string("invalid"), std::string("valid") };
                statuses->forEach([=](auto status) mutable
                {
                    auto config = object{
                        object::pair{std::string("type"), std::string("config")}, 
                        object::pair{std::string("required"), true}, 
                        object::pair{std::string("description"), std::string("Test config")}, 
                        object::pair{std::string("canGenerate"), false}, 
                        object::pair{std::string("status"), std::string("status")}, 
                        object::pair{std::string("attempts"), 0}, 
                        object::pair{std::string("plugin"), std::string("test")}
                    };
                    expect(config->status)->toBe(status);
                }
                );
            }
            );
        }
        );
        describe(std::string("EnvVarMetadata"), [=]() mutable
        {
            it(std::string("should allow nested plugin configurations"), [=]() mutable
            {
                auto metadata = object{
                    object::pair{std::string("plugin1"), object{
                        object::pair{std::string("VAR1"), object{
                            object::pair{std::string("type"), std::string("api_key")}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Plugin 1 API key")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), std::string("missing")}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("plugin1")}
                        }}, 
                        object::pair{std::string("VAR2"), object{
                            object::pair{std::string("type"), std::string("secret")}, 
                            object::pair{std::string("required"), false}, 
                            object::pair{std::string("description"), std::string("Plugin 1 secret")}, 
                            object::pair{std::string("canGenerate"), true}, 
                            object::pair{std::string("status"), std::string("valid")}, 
                            object::pair{std::string("attempts"), 1}, 
                            object::pair{std::string("plugin"), std::string("plugin1")}
                        }}
                    }}, 
                    object::pair{std::string("plugin2"), object{
                        object::pair{std::string("CONFIG_VAR"), object{
                            object::pair{std::string("type"), std::string("config")}, 
                            object::pair{std::string("required"), true}, 
                            object::pair{std::string("description"), std::string("Plugin 2 config")}, 
                            object::pair{std::string("canGenerate"), false}, 
                            object::pair{std::string("status"), std::string("valid")}, 
                            object::pair{std::string("attempts"), 0}, 
                            object::pair{std::string("plugin"), std::string("plugin2")}
                        }}
                    }}
                };
                expect(Object->keys(metadata))->toEqual(array<string>{ std::string("plugin1"), std::string("plugin2") });
                expect(Object->keys(metadata->plugin1))->toEqual(array<string>{ std::string("VAR1"), std::string("VAR2") });
                expect(Object->keys(metadata->plugin2))->toEqual(array<string>{ std::string("CONFIG_VAR") });
                expect(metadata->plugin1["VAR1"]->type)->toBe(std::string("api_key"));
                expect(metadata->plugin2["CONFIG_VAR"]->type)->toBe(std::string("config"));
            }
            );
        }
        );
        describe(std::string("GenerationScript"), [=]() mutable
        {
            it(std::string("should allow valid GenerationScript objects"), [=]() mutable
            {
                auto script = object{
                    object::pair{std::string("variableName"), std::string("SECRET_KEY")}, 
                    object::pair{std::string("pluginName"), std::string("test-plugin")}, 
                    object::pair{std::string("script"), std::string("console.log("generated value");")}, 
                    object::pair{std::string("dependencies"), array<string>{ std::string("crypto") }}, 
                    object::pair{std::string("attempts"), 0}, 
                    object::pair{std::string("status"), std::string("pending")}, 
                    object::pair{std::string("createdAt"), Date->now()}
                };
                expect(script->variableName)->toBe(std::string("SECRET_KEY"));
                expect(script->pluginName)->toBe(std::string("test-plugin"));
                expect(script->script)->toBe(std::string("console.log("generated value");"));
                expect(script->dependencies)->toEqual(array<string>{ std::string("crypto") });
                expect(script->attempts)->toBe(0);
                expect(script->status)->toBe(std::string("pending"));
                expect(type_of(script->createdAt))->toBe(std::string("number"));
            }
            );
            it(std::string("should allow optional fields"), [=]() mutable
            {
                auto script = object{
                    object::pair{std::string("variableName"), std::string("API_KEY")}, 
                    object::pair{std::string("pluginName"), std::string("test-plugin")}, 
                    object::pair{std::string("script"), std::string("console.log("api key");")}, 
                    object::pair{std::string("dependencies"), array<any>()}, 
                    object::pair{std::string("attempts"), 2}, 
                    object::pair{std::string("status"), std::string("failed")}, 
                    object::pair{std::string("createdAt"), Date->now()}, 
                    object::pair{std::string("output"), std::string("Generated output")}, 
                    object::pair{std::string("error"), std::string("Generation failed")}
                };
                expect(script->output)->toBe(std::string("Generated output"));
                expect(script->error)->toBe(std::string("Generation failed"));
            }
            );
            it(std::string("should support all valid statuses"), [=]() mutable
            {
                auto statuses = array<any>{ std::string("pending"), std::string("running"), std::string("success"), std::string("failed") };
                statuses->forEach([=](auto status) mutable
                {
                    auto script = object{
                        object::pair{std::string("variableName"), std::string("TEST_VAR")}, 
                        object::pair{std::string("pluginName"), std::string("test")}, 
                        object::pair{std::string("script"), std::string("console.log("test");")}, 
                        object::pair{std::string("dependencies"), array<any>()}, 
                        object::pair{std::string("attempts"), 0}, 
                        object::pair{std::string("status"), std::string("status")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    };
                    expect(script->status)->toBe(status);
                }
                );
            }
            );
        }
        );
        describe(std::string("GenerationScriptMetadata"), [=]() mutable
        {
            it(std::string("should allow script configurations by script ID"), [=]() mutable
            {
                auto metadata = object{
                    object::pair{std::string("script1"), object{
                        object::pair{std::string("variableName"), std::string("SECRET_KEY")}, 
                        object::pair{std::string("pluginName"), std::string("plugin1")}, 
                        object::pair{std::string("script"), std::string("crypto.randomBytes(32).toString("hex")")}, 
                        object::pair{std::string("dependencies"), array<any>()}, 
                        object::pair{std::string("attempts"), 0}, 
                        object::pair{std::string("status"), std::string("pending")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    }}, 
                    object::pair{std::string("script2"), object{
                        object::pair{std::string("variableName"), std::string("PRIVATE_KEY")}, 
                        object::pair{std::string("pluginName"), std::string("plugin2")}, 
                        object::pair{std::string("script"), std::string("generateKeyPair()")}, 
                        object::pair{std::string("dependencies"), array<string>{ std::string("crypto") }}, 
                        object::pair{std::string("attempts"), 1}, 
                        object::pair{std::string("status"), std::string("success")}, 
                        object::pair{std::string("createdAt"), Date->now()}
                    }}
                };
                expect(Object->keys(metadata))->toEqual(array<string>{ std::string("script1"), std::string("script2") });
                expect(metadata->script1->variableName)->toBe(std::string("SECRET_KEY"));
                expect(metadata->script2->status)->toBe(std::string("success"));
            }
            );
        }
        );
        describe(std::string("ValidationResult"), [=]() mutable
        {
            it(std::string("should allow valid ValidationResult objects"), [=]() mutable
            {
                auto validResult = object{
                    object::pair{std::string("isValid"), true}, 
                    object::pair{std::string("details"), std::string("Validation passed successfully")}
                };
                expect(validResult->isValid)->toBe(true);
                expect(validResult->details)->toBe(std::string("Validation passed successfully"));
                expect(validResult->error)->toBeUndefined();
            }
            );
            it(std::string("should allow invalid ValidationResult objects"), [=]() mutable
            {
                auto invalidResult = object{
                    object::pair{std::string("isValid"), false}, 
                    object::pair{std::string("error"), std::string("Validation failed")}, 
                    object::pair{std::string("details"), std::string("API key format is incorrect")}
                };
                expect(invalidResult->isValid)->toBe(false);
                expect(invalidResult->error)->toBe(std::string("Validation failed"));
                expect(invalidResult->details)->toBe(std::string("API key format is incorrect"));
            }
            );
            it(std::string("should allow minimal ValidationResult objects"), [=]() mutable
            {
                auto minimalValid = object{
                    object::pair{std::string("isValid"), true}
                };
                auto minimalInvalid = object{
                    object::pair{std::string("isValid"), false}
                };
                expect(minimalValid->isValid)->toBe(true);
                expect(minimalInvalid->isValid)->toBe(false);
            }
            );
        }
        );
        describe(std::string("EnvVarUpdate"), [=]() mutable
        {
            it(std::string("should allow valid EnvVarUpdate objects"), [=]() mutable
            {
                auto update = object{
                    object::pair{std::string("pluginName"), std::string("test-plugin")}, 
                    object::pair{std::string("variableName"), std::string("API_KEY")}, 
                    object::pair{std::string("value"), std::string("sk-test123")}
                };
                expect(update->pluginName)->toBe(std::string("test-plugin"));
                expect(update->variableName)->toBe(std::string("API_KEY"));
                expect(update->value)->toBe(std::string("sk-test123"));
            }
            );
        }
        );
        describe(std::string("Type compatibility"), [=]() mutable
        {
            it(std::string("should allow empty metadata objects"), [=]() mutable
            {
                auto emptyEnvVars = object{};
                auto emptyScripts = object{};
                expect(Object->keys(emptyEnvVars))->toEqual(array<any>());
                expect(Object->keys(emptyScripts))->toEqual(array<any>());
            }
            );
            it(std::string("should work with all credential types"), [=]() mutable
            {
                auto credentialConfig = object{
                    object::pair{std::string("type"), std::string("credential")}, 
                    object::pair{std::string("required"), true}, 
                    object::pair{std::string("description"), std::string("Database credentials")}, 
                    object::pair{std::string("canGenerate"), false}, 
                    object::pair{std::string("status"), std::string("missing")}, 
                    object::pair{std::string("attempts"), 0}, 
                    object::pair{std::string("plugin"), std::string("database-plugin")}
                };
                expect(credentialConfig->type)->toBe(std::string("credential"));
            }
            );
        }
        );
    }
    );
}

MAIN
