#include "validation.test.h"

any mockFetch = vi->fn();

void Main(void)
{
    global->fetch = as<any>(mockFetch);
    describe(std:("validation"), [=]() mutable
    {
        beforeEach([=]() mutable
        {
            vi->clearAllMocks();
            mockFetch->mockReset();
        }
        );
        describe(std:("validateEnvVar"), [=]() mutable
        {
            it(std:("should return invalid for empty value"), [=]() mutable
            {
                auto result = std::async([=]() { validateEnvVar(std:("TEST_VAR"), string_empty, std:("api_key")); });
                expect(result->isValid)->toBe(false);
                expect(result->error)->toBe(std:("Environment variable value is empty"));
            }
            );
            it(std:("should return invalid for whitespace-only value"), [=]() mutable
            {
                auto result = std::async([=]() { validateEnvVar(std:("TEST_VAR"), std:("   "), std:("api_key")); });
                expect(result->isValid)->toBe(false);
                expect(result->error)->toBe(std:("Environment variable value is empty"));
            }
            );
            it(std:("should use basic validation for unknown types"), [=]() mutable
            {
                auto loggerSpy = vi->spyOn(logger, std:("warn"));
                auto result = std::async([=]() { validateEnvVar(std:("TEST_VAR"), std:("test-value"), std:("unknown_type")); });
                expect(result->isValid)->toBe(true);
                expect(result->details)->toBe(std:("Basic validation passed - value is present"));
                expect(loggerSpy)->toHaveBeenCalledWith(std:("No specific validation strategy found for TEST_VAR, using basic validation"));
                loggerSpy->mockRestore();
            }
            );
            it(std:("should handle validation errors gracefully"), [=]() mutable
            {
                auto originalStrategy = validationStrategies["api_key"]["openai"];
                validationStrategies["api_key"]["openai"] = vi->fn()->mockRejectedValue(std::make_shared<Error>(std:("Test error")));
                auto loggerSpy = vi->spyOn(logger, std:("error"));
                auto result = std::async([=]() { validateEnvVar(std:("TEST_VAR"), std:("test-value"), std:("api_key"), std:("api_key:openai")); });
                expect(result->isValid)->toBe(false);
                expect(result->error)->toBe(std:("Validation failed due to unexpected error"));
                expect(result->details)->toBe(std:("Test error"));
                expect(loggerSpy)->toHaveBeenCalledWith(std:("Error validating environment variable TEST_VAR:"), std::make_shared<Error>(std:("Test error")));
                validationStrategies["api_key"]["openai"] = originalStrategy;
                loggerSpy->mockRestore();
            }
            );
            it(std:("should use specific validation strategy when provided"), [=]() mutable
            {
                mockFetch->mockResolvedValue(object{
                    object::pair{std:("ok"), true}, 
                    object::pair{std:("status"), 200}
                });
                auto result = std::async([=]() { validateEnvVar(std:("OPENAI_API_KEY"), std:("sk-test123"), std:("api_key"), std:("api_key:openai")); });
                expect(result->isValid)->toBe(true);
                expect(result->details)->toBe(std:("OpenAI API key validated successfully"));
            }
            );
        }
        );
        describe(std:("validationStrategies"), [=]() mutable
        {
            describe(std:("api_key"), [=]() mutable
            {
                describe(std:("openai"), [=]() mutable
                {
                    it(std:("should return valid for successful API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), true}, 
                            object::pair{std:("status"), 200}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["openai"](std:("test-key")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("OpenAI API key validated successfully"));
                        expect(mockFetch)->toHaveBeenCalledWith(std:("https://api.openai.com/v1/models"), object{
                            object::pair{std:("headers"), object{
                                object::pair{std:("Authorization"), std:("Bearer test-key")}, 
                                object::pair{std:("Content-Type"), std:("application/json")}
                            }}
                        });
                    }
                    );
                    it(std:("should return invalid for failed API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), false}, 
                            object::pair{std:("status"), 401}, 
                            object::pair{std:("text"), vi->fn()->mockResolvedValue(std:("Unauthorized"))}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["openai"](std:("invalid-key")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("OpenAI API validation failed: 401"));
                        expect(result->details)->toBe(std:("Unauthorized"));
                    }
                    );
                    it(std:("should handle network errors"), [=]() mutable
                    {
                        mockFetch->mockRejectedValue(std::make_shared<Error>(std:("Network error")));
                        auto result = std::async([=]() { validationStrategies["api_key"]["openai"](std:("test-key")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Failed to validate OpenAI API key"));
                        expect(result->details)->toBe(std:("Network error"));
                    }
                    );
                }
                );
                describe(std:("groq"), [=]() mutable
                {
                    it(std:("should return valid for successful API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), true}, 
                            object::pair{std:("status"), 200}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["groq"](std:("test-key")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Groq API key validated successfully"));
                        expect(mockFetch)->toHaveBeenCalledWith(std:("https://api.groq.com/openai/v1/models"), object{
                            object::pair{std:("headers"), object{
                                object::pair{std:("Authorization"), std:("Bearer test-key")}, 
                                object::pair{std:("Content-Type"), std:("application/json")}
                            }}
                        });
                    }
                    );
                    it(std:("should return invalid for failed API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), false}, 
                            object::pair{std:("status"), 403}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["groq"](std:("invalid-key")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Groq API validation failed: 403"));
                    }
                    );
                    it(std:("should handle network errors"), [=]() mutable
                    {
                        mockFetch->mockRejectedValue(std::make_shared<Error>(std:("Connection timeout")));
                        auto result = std::async([=]() { validationStrategies["api_key"]["groq"](std:("test-key")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Failed to validate Groq API key"));
                        expect(result->details)->toBe(std:("Connection timeout"));
                    }
                    );
                }
                );
                describe(std:("anthropic"), [=]() mutable
                {
                    it(std:("should return valid for successful API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), true}, 
                            object::pair{std:("status"), 200}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["anthropic"](std:("test-key")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Anthropic API key validated successfully"));
                        expect(mockFetch)->toHaveBeenCalledWith(std:("https://api.anthropic.com/v1/messages"), object{
                            object::pair{std:("method"), std:("POST")}, 
                            object::pair{std:("headers"), object{
                                object::pair{std:("x-api-key"), std:("test-key")}, 
                                object::pair{std:("Content-Type"), std:("application/json")}, 
                                object::pair{std:("anthropic-version"), std:("2023-06-01")}
                            }}, 
                            object::pair{std:("body"), JSON->stringify(object{
                                object::pair{std:("model"), std:("claude-3-haiku-20240307")}, 
                                object::pair{std:("max_tokens"), 1}, 
                                object::pair{std:("messages"), array<object>{ object{
                                    object::pair{std:("role"), std:("user")}, 
                                    object::pair{std:("content"), std:("test")}
                                } }}
                            })}
                        });
                    }
                    );
                    it(std:("should return valid for 400 status (expected for minimal test)"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), false}, 
                            object::pair{std:("status"), 400}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["anthropic"](std:("test-key")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Anthropic API key validated successfully"));
                    }
                    );
                    it(std:("should return invalid for unauthorized response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), false}, 
                            object::pair{std:("status"), 401}
                        });
                        auto result = std::async([=]() { validationStrategies["api_key"]["anthropic"](std:("invalid-key")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Anthropic API validation failed: 401"));
                    }
                    );
                    it(std:("should handle network errors"), [=]() mutable
                    {
                        mockFetch->mockRejectedValue(std::make_shared<Error>(std:("DNS resolution failed")));
                        auto result = std::async([=]() { validationStrategies["api_key"]["anthropic"](std:("test-key")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Failed to validate Anthropic API key"));
                        expect(result->details)->toBe(std:("DNS resolution failed"));
                    }
                    );
                }
                );
            }
            );
            describe(std:("url"), [=]() mutable
            {
                describe(std:("webhook"), [=]() mutable
                {
                    it(std:("should return valid for successful webhook response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("status"), 200}
                        });
                        auto result = std::async([=]() { validationStrategies["url"]["webhook"](std:("https://example.com/webhook")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Webhook URL is reachable"));
                        expect(mockFetch)->toHaveBeenCalledWith(std:("https://example.com/webhook"), object{
                            object::pair{std:("method"), std:("POST")}, 
                            object::pair{std:("headers"), object{
                                object::pair{std:("Content-Type"), std:("application/json")}
                            }}, 
                            object::pair{std:("body"), JSON->stringify(object{
                                object::pair{std:("test"), true}
                            })}
                        });
                    }
                    );
                    it(std:("should return valid for client error responses (< 500)"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("status"), 404}
                        });
                        auto result = std::async([=]() { validationStrategies["url"]["webhook"](std:("https://example.com/webhook")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Webhook URL is reachable"));
                    }
                    );
                    it(std:("should return invalid for server error responses (>= 500)"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("status"), 500}
                        });
                        auto result = std::async([=]() { validationStrategies["url"]["webhook"](std:("https://example.com/webhook")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Webhook URL returned server error: 500"));
                    }
                    );
                    it(std:("should handle network errors"), [=]() mutable
                    {
                        mockFetch->mockRejectedValue(std::make_shared<Error>(std:("ECONNREFUSED")));
                        auto result = std::async([=]() { validationStrategies["url"]["webhook"](std:("https://example.com/webhook")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Webhook URL is not reachable"));
                        expect(result->details)->toBe(std:("ECONNREFUSED"));
                    }
                    );
                }
                );
                describe(std:("api_endpoint"), [=]() mutable
                {
                    it(std:("should return valid for successful API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), true}, 
                            object::pair{std:("status"), 200}
                        });
                        auto result = std::async([=]() { validationStrategies["url"]["api_endpoint"](std:("https://api.example.com")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("API endpoint is reachable"));
                        expect(mockFetch)->toHaveBeenCalledWith(std:("https://api.example.com"));
                    }
                    );
                    it(std:("should return invalid for failed API response"), [=]() mutable
                    {
                        mockFetch->mockResolvedValue(object{
                            object::pair{std:("ok"), false}, 
                            object::pair{std:("status"), 404}
                        });
                        auto result = std::async([=]() { validationStrategies["url"]["api_endpoint"](std:("https://api.example.com")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("API endpoint returned error: 404"));
                    }
                    );
                    it(std:("should handle network errors"), [=]() mutable
                    {
                        mockFetch->mockRejectedValue(std::make_shared<Error>(std:("Timeout")));
                        auto result = std::async([=]() { validationStrategies["url"]["api_endpoint"](std:("https://api.example.com")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("API endpoint is not reachable"));
                        expect(result->details)->toBe(std:("Timeout"));
                    }
                    );
                }
                );
            }
            );
            describe(std:("credential"), [=]() mutable
            {
                describe(std:("database_url"), [=]() mutable
                {
                    it(std:("should return valid for proper database URL"), [=]() mutable
                    {
                        auto result = std::async([=]() { validationStrategies["credential"]["database_url"](std:("postgresql://user:pass@localhost:5432/db")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Database URL format is valid"));
                    }
                    );
                    it(std:("should return valid for MongoDB URL"), [=]() mutable
                    {
                        auto result = std::async([=]() { validationStrategies["credential"]["database_url"](std:("mongodb://user:pass@localhost:27017/db")); });
                        expect(result->isValid)->toBe(true);
                        expect(result->details)->toBe(std:("Database URL format is valid"));
                    }
                    );
                    it(std:("should return invalid for malformed URL"), [=]() mutable
                    {
                        auto result = std::async([=]() { validationStrategies["credential"]["database_url"](std:("not-a-url")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Invalid database URL format"));
                    }
                    );
                    it(std:("should return invalid for URL without hostname"), [=]() mutable
                    {
                        auto result = std::async([=]() { validationStrategies["credential"]["database_url"](std:("postgresql://")); });
                        expect(result->isValid)->toBe(false);
                        expect(result->error)->toBe(std:("Invalid database URL format"));
                    }
                    );
                }
                );
            }
            );
            describe(std:("private_key"), [=]() mutable
            {
                it(std:("should have rsa validation strategy"), [=]() mutable
                {
                    expect(validationStrategies["private_key"]["rsa"])->toBeDefined();
                    expect(type_of(validationStrategies["private_key"]["rsa"]))->toBe(std:("function"));
                }
                );
                it(std:("should have ed25519 validation strategy"), [=]() mutable
                {
                    expect(validationStrategies["private_key"]["ed25519"])->toBeDefined();
                    expect(type_of(validationStrategies["private_key"]["ed25519"]))->toBe(std:("function"));
                }
                );
            }
            );
        }
        );
    }
    );
}

MAIN
