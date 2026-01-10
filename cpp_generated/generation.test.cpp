#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-env/generation.test.h"

void Main(void)
{
    describe(std::string("generation"), [=]() mutable
    {
        describe(std::string("canGenerateEnvVar"), [=]() mutable
        {
            it(std::string("should return true for private key types"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("PRIVATE_KEY"), std::string("private_key")))->toBe(true);
                expect(canGenerateEnvVar(std::string("RSA_PRIVATE_KEY"), std::string("private_key")))->toBe(true);
                expect(canGenerateEnvVar(std::string("ED25519_PRIVATE_KEY"), std::string("private_key")))->toBe(true);
            }
            );
            it(std::string("should return true for variables with private_key in name"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("MY_PRIVATE_KEY"), std::string("config")))->toBe(true);
                expect(canGenerateEnvVar(std::string("APP_PRIVATE_KEY"), std::string("secret")))->toBe(true);
            }
            );
            it(std::string("should return true for variables with private key in description"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("CRYPTO_KEY"), std::string("config"), std::string("This is a private key for encryption")))->toBe(true);
            }
            );
            it(std::string("should return true for secret types (except API keys)"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("SECRET_KEY"), std::string("secret")))->toBe(true);
                expect(canGenerateEnvVar(std::string("JWT_SECRET"), std::string("secret")))->toBe(true);
                expect(canGenerateEnvVar(std::string("ENCRYPTION_KEY"), std::string("secret")))->toBe(true);
            }
            );
            it(std::string("should return false for API keys"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("OPENAI_API_KEY"), std::string("secret")))->toBe(false);
                expect(canGenerateEnvVar(std::string("API_KEY"), std::string("secret")))->toBe(false);
                expect(canGenerateEnvVar(std::string("MY_KEY"), std::string("secret"), std::string("This is an API key")))->toBe(false);
            }
            );
            it(std::string("should return true for config values like port and database_name"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("PORT"), std::string("config")))->toBe(true);
                expect(canGenerateEnvVar(std::string("SERVER_PORT"), std::string("config")))->toBe(true);
                expect(canGenerateEnvVar(std::string("DATABASE_NAME"), std::string("config")))->toBe(true);
                expect(canGenerateEnvVar(std::string("DB_NAME"), std::string("config")))->toBe(false);
            }
            );
            it(std::string("should return true for UUID and ID variables"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("UUID"), std::string("config")))->toBe(true);
                expect(canGenerateEnvVar(std::string("SESSION_ID"), std::string("config")))->toBe(true);
                expect(canGenerateEnvVar(std::string("UNIQUE_ID"), std::string("config")))->toBe(true);
            }
            );
            it(std::string("should return false for other types"), [=]() mutable
            {
                expect(canGenerateEnvVar(std::string("API_URL"), std::string("url")))->toBe(false);
                expect(canGenerateEnvVar(std::string("CONFIG_VALUE"), std::string("config")))->toBe(false);
                expect(canGenerateEnvVar(std::string("UNKNOWN_VAR"), as<any>(std::string("unknown"))))->toBe(false);
            }
            );
        }
        );
        describe(std::string("generateScript"), [=]() mutable
        {
            it(std::string("should generate RSA private key script by default"), [=]() mutable
            {
                auto script = generateScript(std::string("PRIVATE_KEY"), std::string("private_key"), std::string("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("generateKeyPairSync('rsa'"));
                expect(script->script)->toContain(std::string("modulusLength: 2048"));
                expect(script->variableName)->toBe(std::string("PRIVATE_KEY"));
                expect(script->pluginName)->toBe(std::string("test-plugin"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std::string("should generate Ed25519 private key script when specified"), [=]() mutable
            {
                auto script = generateScript(std::string("ED25519_PRIVATE_KEY"), std::string("private_key"), std::string("test-plugin"), std::string("Ed25519 key for signing"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("generateKeyPairSync('ed25519'"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std::string("should generate UUID script for UUID variables"), [=]() mutable
            {
                auto script = generateScript(std::string("SESSION_UUID"), std::string("config"), std::string("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("uuidv4()"));
                expect(script->dependencies)->toEqual(array<string>{ std::string("uuid") });
            }
            );
            it(std::string("should generate JWT secret script for JWT variables"), [=]() mutable
            {
                auto script = generateScript(std::string("JWT_SECRET"), std::string("secret"), std::string("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("randomBytes(32).toString('base64url')"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std::string("should generate hex secret script by default for secrets"), [=]() mutable
            {
                auto script = generateScript(std::string("SECRET_KEY"), std::string("secret"), std::string("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("randomBytes(32).toString('hex')"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std::string("should generate base64 secret script when specified"), [=]() mutable
            {
                auto script = generateScript(std::string("BASE64_SECRET"), std::string("secret"), std::string("test-plugin"), std::string("A base64 encoded secret"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("randomBytes(32).toString('base64')"));
            }
            );
            it(std::string("should generate port script for port variables"), [=]() mutable
            {
                auto script = generateScript(std::string("SERVER_PORT"), std::string("config"), std::string("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("Math.floor(Math.random()"));
                expect(script->script)->toContain(std::string("3000"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std::string("should generate database name script for database variables"), [=]() mutable
            {
                auto script = generateScript(std::string("DATABASE_NAME"), std::string("config"), std::string("test-plugin"));
                expect(script)->not->toBeNull();
                expect(script->script)->toContain(std::string("app_db_"));
                expect(script->script)->toContain(std::string("Date.now()"));
                expect(script->dependencies)->toEqual(array<any>());
            }
            );
            it(std::string("should return null for unsupported variables"), [=]() mutable
            {
                auto loggerSpy = vi->spyOn(logger, std::string("warn"));
                auto script = generateScript(std::string("API_URL"), std::string("url"), std::string("test-plugin"));
                expect(script)->toBeNull();
                expect(loggerSpy)->toHaveBeenCalledWith(std::string("No generation script available for API_URL of type url"));
                loggerSpy->mockRestore();
            }
            );
            it(std::string("should set correct metadata in generated script"), [=]() mutable
            {
                auto script = generateScript(std::string("TEST_SECRET"), std::string("secret"), std::string("my-plugin"));
                expect(script)->not->toBeNull();
                expect(script->variableName)->toBe(std::string("TEST_SECRET"));
                expect(script->pluginName)->toBe(std::string("my-plugin"));
                expect(script->attempts)->toBe(0);
                expect(script->status)->toBe(std::string("pending"));
                expect(script->createdAt)->toBeGreaterThan(0);
            }
            );
        }
        );
        describe(std::string("getGenerationDescription"), [=]() mutable
        {
            it(std::string("should return correct description for RSA private keys"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("PRIVATE_KEY"), std::string("private_key"));
                expect(desc)->toBe(std::string("Generate a new RSA private key for cryptographic operations"));
            }
            );
            it(std::string("should return correct description for Ed25519 private keys"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("ED25519_PRIVATE_KEY"), std::string("private_key"));
                expect(desc)->toBe(std::string("Generate a new Ed25519 private key for cryptographic operations"));
            }
            );
            it(std::string("should return correct description for UUIDs"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("SESSION_UUID"), std::string("config"));
                expect(desc)->toBe(std::string("Generate a new UUID (Universally Unique Identifier)"));
            }
            );
            it(std::string("should return correct description for JWT secrets"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("JWT_SECRET"), std::string("secret"));
                expect(desc)->toBe(std::string("Generate a secure secret for JWT token signing"));
            }
            );
            it(std::string("should return correct description for general secrets"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("SECRET_KEY"), std::string("secret"));
                expect(desc)->toBe(std::string("Generate a cryptographically secure random secret"));
            }
            );
            it(std::string("should return correct description for ports"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("SERVER_PORT"), std::string("config"));
                expect(desc)->toBe(std::string("Generate a random port number for the application"));
            }
            );
            it(std::string("should return correct description for database names"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("DATABASE_NAME"), std::string("config"));
                expect(desc)->toBe(std::string("Generate a unique database name"));
            }
            );
            it(std::string("should return generic description for unknown types"), [=]() mutable
            {
                auto desc = getGenerationDescription(std::string("UNKNOWN_VAR"), std::string("unknown"));
                expect(desc)->toBe(std::string("Generate a value for this environment variable"));
            }
            );
        }
        );
        describe(std::string("generationTemplates"), [=]() mutable
        {
            it(std::string("should have RSA private key template"), [=]() mutable
            {
                expect(generationTemplates["private_key"]["rsa"])->toContain(std::string("generateKeyPairSync('rsa'"));
                expect(generationTemplates["private_key"]["rsa"])->toContain(std::string("modulusLength: 2048"));
            }
            );
            it(std::string("should have Ed25519 private key template"), [=]() mutable
            {
                expect(generationTemplates["private_key"]["ed25519"])->toContain(std::string("generateKeyPairSync('ed25519'"));
            }
            );
            it(std::string("should have UUID template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["uuid"])->toContain(std::string("uuidv4()"));
            }
            );
            it(std::string("should have hex secret template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["hex_32"])->toContain(std::string("randomBytes(32).toString('hex')"));
            }
            );
            it(std::string("should have base64 secret template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["base64_32"])->toContain(std::string("randomBytes(32).toString('base64')"));
            }
            );
            it(std::string("should have JWT secret template"), [=]() mutable
            {
                expect(generationTemplates["secret"]["jwt_secret"])->toContain(std::string("randomBytes(32).toString('base64url')"));
            }
            );
            it(std::string("should have port template"), [=]() mutable
            {
                expect(generationTemplates["config"]["port"])->toContain(std::string("Math.floor(Math.random()"));
            }
            );
            it(std::string("should have database name template"), [=]() mutable
            {
                expect(generationTemplates["config"]["database_name"])->toContain(std::string("app_db_"));
            }
            );
        }
        );
        describe(std::string("generationDependencies"), [=]() mutable
        {
            it(std::string("should have correct dependencies for private keys"), [=]() mutable
            {
                expect(generationDependencies["private_key"])->toEqual(array<any>());
            }
            );
            it(std::string("should have correct dependencies for UUID"), [=]() mutable
            {
                expect(generationDependencies["secret"]["uuid"])->toEqual(array<string>{ std::string("uuid") });
            }
            );
            it(std::string("should have correct dependencies for other secrets"), [=]() mutable
            {
                expect(generationDependencies["secret"]["hex_32"])->toEqual(array<any>());
                expect(generationDependencies["secret"]["base64_32"])->toEqual(array<any>());
                expect(generationDependencies["secret"]["jwt_secret"])->toEqual(array<any>());
            }
            );
            it(std::string("should have correct dependencies for config values"), [=]() mutable
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
