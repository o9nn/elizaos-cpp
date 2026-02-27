#include "generation.test.h"

void Main(void)
{
    describe(std:("generation"), [=]() mutable
    {
        describe(std:("canGenerateEnvVar"), [=]() mutable
        {
            it(std:("should return true for private key types"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("PRIVATE_KEY"), std:("private_key")))->toBe(true);
                expect(canGenerateEnvVar(std:("RSA_PRIVATE_KEY"), std:("private_key")))->toBe(true);
                expect(canGenerateEnvVar(std:("ED25519_PRIVATE_KEY"), std:("private_key")))->toBe(true);
            }
            );
            it(std:("should return true for variables with private_key in name"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("MY_PRIVATE_KEY"), std:("config")))->toBe(true);
                expect(canGenerateEnvVar(std:("APP_PRIVATE_KEY"), std:("secret")))->toBe(true);
            }
            );
            it(std:("should return true for variables with private key in description"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("CRYPTO_KEY"), std:("config"), std:("This is a private key for encryption")))->toBe(true);
            }
            );
            it(std:("should return true for secret types (except API keys)"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("SECRET_KEY"), std:("secret")))->toBe(true);
                expect(canGenerateEnvVar(std:("JWT_SECRET"), std:("secret")))->toBe(true);
                expect(canGenerateEnvVar(std:("ENCRYPTION_KEY"), std:("secret")))->toBe(true);
            }
            );
            it(std:("should return false for API keys"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("OPENAI_API_KEY"), std:("secret")))->toBe(false);
                expect(canGenerateEnvVar(std:("API_KEY"), std:("secret")))->toBe(false);
                expect(canGenerateEnvVar(std:("MY_KEY"), std:("secret"), std:("This is an API key")))->toBe(false);
            }
            );
            it(std:("should return true for config values like port and database_name"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("PORT"), std:("config")))->toBe(true);
                expect(canGenerateEnvVar(std:("SERVER_PORT"), std:("config")))->toBe(true);
                expect(canGenerateEnvVar(std:("DATABASE_NAME"), std:("config")))->toBe(true);
                expect(canGenerateEnvVar(std:("DB_NAME"), std:("config")))->toBe(false);
            }
            );
            it(std:("should return true for UUID and ID variables"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("UUID"), std:("config")))->toBe(true);
                expect(canGenerateEnvVar(std:("SESSION_ID"), std:("config")))->toBe(true);
                expect(canGenerateEnvVar(std:("UNIQUE_ID"), std:("config")))->toBe(true);
            }
            );
            it(std:("should return false for other types"), [=]() mutable
            {
                expect(canGenerateEnvVar(std:("API_URL"), std:("url")))->toBe(false);
                expect(canGenerateEnvVar(std:("CONFIG_VALUE"), std:("config")))->toBe(false);
                expect(canGenerateEnvVar(std:("UNKNOWN_VAR"), as<any>(std:("unknown"))))->toBe(false);
            }
            );
        }
        );
        describe(std:("generateScript"), [=]() mutable
        {
            it(std:("should generate RSA private key script by default"), [=]() mutable
            {
                auto script = generateScript(std:("PRIVATE_KEY"), std:("private_key"), std:("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("generateKeyPairSync('rsa'"));
                expect(script->script)->toContain(std:("modulusLength: 2048"));
                expect(script->variableName)->toBe(std:("PRIVATE_KEY"));
                expect(script->pluginName)->toBe(std:("test-plugin"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std:("should generate Ed25519 private key script when specified"), [=]() mutable
            {
                auto script = generateScript(std:("ED25519_PRIVATE_KEY"), std:("private_key"), std:("test-plugin"), std:("Ed25519 key for signing"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("generateKeyPairSync('ed25519'"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std:("should generate UUID script for UUID variables"), [=]() mutable
            {
                auto script = generateScript(std:("SESSION_UUID"), std:("config"), std:("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("uuidv4()"));
                expect(script->dependencies)->toEqual(array<string>{ std:("uuid") });
            }
            );
            it(std:("should generate JWT secret script for JWT variables"), [=]() mutable
            {
                auto script = generateScript(std:("JWT_SECRET"), std:("secret"), std:("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("randomBytes(32).toString('base64url')"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std:("should generate hex secret script by default for secrets"), [=]() mutable
            {
                auto script = generateScript(std:("SECRET_KEY"), std:("secret"), std:("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("randomBytes(32).toString('hex')"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std:("should generate base64 secret script when specified"), [=]() mutable
            {
                auto script = generateScript(std:("BASE64_SECRET"), std:("secret"), std:("test-plugin"), std:("A base64 encoded secret"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("randomBytes(32).toString('base64')"));
            }
            );
            it(std:("should generate port script for port variables"), [=]() mutable
            {
                auto script = generateScript(std:("SERVER_PORT"), std:("config"), std:("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("Math.floor(Math.random()"));
                expect(script->script)->toContain(std:("3000"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std:("should generate database name script for database variables"), [=]() mutable
            {
                auto script = generateScript(std:("DATABASE_NAME"), std:("config"), std:("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std:("app_db_"));
                expect(script->script)->toContain(std:("Date.now()"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std:("should return null for unsupported variables"), [=]() mutable
            {
                auto loggerSpy = vi->spyOn(logger, std:("warn"));
                auto script = generateScript(std:("API_URL"), std:("url"), std:("test-plugin"));
                expect(script)->toBeNull();
                expect(loggerSpy)->toHaveBeenCalledWith(std:("No generation script available for API_URL of type url"));
                loggerSpy->mockRestore();
            }
            );
            it(std:("should set correct metadata in generated script"), [=]() mutable
            {
                auto script = generateScript(std:("TEST_SECRET"), std:("secret"), std:("my-plugin"));
                expect(script)->not->toBeNull();
                expect(script->variableName)->toBe(std:("TEST_SECRET"));
                expect(script->pluginName)->toBe(std:("my-plugin"));
                expect(script->attempts)->toBe(0);
                expect(script->status)->toBe(std:("pending"));
                expect(script->createdAt)->toBeGreaterThan(0);
            }
            );
        }
        );
        describe(std:("getGenerationDescription"), [=]() mutable
        {
            it(std:("should return correct description for RSA private keys"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("PRIVATE_KEY"), std:("private_key"));
                expect(desc)->toBe(std:("Generate a new RSA private key for cryptographic operations"));
            }
            );
            it(std:("should return correct description for Ed25519 private keys"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("ED25519_PRIVATE_KEY"), std:("private_key"));
                expect(desc)->toBe(std:("Generate a new Ed25519 private key for cryptographic operations"));
            }
            );
            it(std:("should return correct description for UUIDs"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("SESSION_UUID"), std:("config"));
                expect(desc)->toBe(std:("Generate a new UUID (Universally Unique Identifier)"));
            }
            );
            it(std:("should return correct description for JWT secrets"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("JWT_SECRET"), std:("secret"));
                expect(desc)->toBe(std:("Generate a secure secret for JWT token signing"));
            }
            );
            it(std:("should return correct description for general secrets"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("SECRET_KEY"), std:("secret"));
                expect(desc)->toBe(std:("Generate a cryptographically secure random secret"));
            }
            );
            it(std:("should return correct description for ports"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("SERVER_PORT"), std:("config"));
                expect(desc)->toBe(std:("Generate a random port number for the application"));
            }
            );
            it(std:("should return correct description for database names"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("DATABASE_NAME"), std:("config"));
                expect(desc)->toBe(std:("Generate a unique database name"));
            }
            );
            it(std:("should return generic description for unknown types"), [=]() mutable
            {
                auto desc = getGenerationDescription(std:("UNKNOWN_VAR"), std:("unknown"));
                expect(desc)->toBe(std:("Generate a value for this environment variable"));
            }
            );
        }
        );
        describe(std:("generationTemplates"), [=]() mutable
        {
            it(std:("should have RSA private key template"), [=]() mutable
            {
                expect(generationTemplates["private_key"]["rsa"])->toContain(std:("generateKeyPairSync('rsa'"));
                expect(generationTemplates["private_key"]["rsa"])->toContain(std:("modulusLength: 2048"));
            }
            );
            it(std:("should have Ed25519 private key template"), [=]() mutable
            {
                expect(generationTemplates["private_key"]["ed25519"])->toContain(std:("generateKeyPairSync('ed25519'"));
            }
            );
            it(std:("should have UUID template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["uuid"])->toContain(std:("uuidv4()"));
            }
            );
            it(std:("should have hex secret template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["hex_32"])->toContain(std:("randomBytes(32).toString('hex')"));
            }
            );
            it(std:("should have base64 secret template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["base64_32"])->toContain(std:("randomBytes(32).toString('base64')"));
            }
            );
            it(std:("should have JWT secret template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["jwt_secret"])->toContain(std:("randomBytes(32).toString('base64url')"));
            }
            );
            it(std:("should have port template"), [=]() mutable
            {
                expect(generationTemplates["config"]["port"])->toContain(std:("Math.floor(Math.random()"));
            }
            );
            it(std:("should have database name template"), [=]() mutable
            {
                expect(generationTemplates["config"]["database_name"])->toContain(std:("app_db_"));
            }
            );
        }
        );
        describe(std:("generationDependencies"), [=]() mutable
        {
            it(std:("should have correct dependencies for private keys"), [=]() mutable
            {
                expect(generationDependencies["private_key"])->toEqual(array<any>());
            }
            );
            it(std:("should have correct dependencies for UUID"), [=]() mutable
            {
                expect(generationDependencies["secret"]["uuid"])->toEqual(array<string>{ std:("uuid") });
            }
            );
            it(std:("should have correct dependencies for other secrets"), [=]() mutable
            {
                expect(generationDependencies["secret"]["hex_32"])->toEqual(array<any>());
                expect(generationDependencies["secret"]["base64_32"])->toEqual(array<any>());
                expect(generationDependencies["secret"]["jwt_secret"])->toEqual(array<any>());
            }
            );
            it(std:("should have correct dependencies for config values"), [=]() mutable
            {
                expect(generationDependencies["config"]["port"])->toEqual(array<any>());
                expect(generationDependencies["config"]["database_name"])->toEqual(array<any>());
            }
            );
        }
        );
    }
    );
}

MAIN
