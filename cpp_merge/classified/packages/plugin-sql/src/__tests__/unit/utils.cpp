#include "utils.test.h"

void Main(void)
{
    describe(std:("Utils"), [=]() mutable
    {
        describe(std:("expandTildePath"), [=]() mutable
        {
            it(std:("should expand paths starting with ~"), [=]() mutable
            {
                auto result = expandTildePath(std:("~/test/path"));
                expect(result)->toBe(path->join(process->cwd(), std:("test/path")));
            }
            );
            it(std:("should return unchanged paths not starting with ~"), [=]() mutable
            {
                auto result = expandTildePath(std:("/absolute/path"));
                expect(result)->toBe(std:("/absolute/path"));
            }
            );
            it(std:("should handle empty strings"), [=]() mutable
            {
                auto result = expandTildePath(string_empty);
                expect(result)->toBe(string_empty);
            }
            );
            it(std:("should handle just tilde"), [=]() mutable
            {
                auto result = expandTildePath(std:("~"));
                expect(result)->toBe(process->cwd());
            }
            );
        }
        );
        describe(std:("resolveEnvFile"), [=]() mutable
        {
            it(std:("should find .env in current directory if it exists"), [=]() mutable
            {
                auto result = resolveEnvFile();
                expect(result)->toMatch((new RegExp(std:("\.env"))));
            }
            );
            it(std:("should return .env path even if not found"), [=]() mutable
            {
                auto testDir = std:("/some/nonexistent/path");
                auto result = resolveEnvFile(testDir);
                expect(result)->toBe(path->join(testDir, std:(".env")));
            }
            );
        }
        );
        describe(std:("resolvePgliteDir"), [=]() mutable
        {
            shared<any> originalEnv;
            beforeEach([=]() mutable
            {
                originalEnv = process->env->PGLITE_DATA_DIR;
                process->env.Delete("PGLITE_DATA_DIR");
            }
            );
            afterEach([=]() mutable
            {
                if (originalEnv == undefined) {
                    process->env.Delete("PGLITE_DATA_DIR");
                } else {
                    process->env->PGLITE_DATA_DIR = originalEnv;
                }
            }
            );
            it(std:("should prioritize dir argument"), [=]() mutable
            {
                auto result = resolvePgliteDir(std:("/custom/dir"));
                expect(result)->toBe(std:("/custom/dir"));
            }
            );
            it(std:("should use PGLITE_DATA_DIR env var if no dir provided"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = std:("/env/pglite/dir");
                auto result = resolvePgliteDir();
                expect(result)->toBe(std:("/env/pglite/dir"));
            }
            );
            it(std:("should use default .eliza/.elizadb dir if no dir or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                expect(result)->toBe(path->join(process->cwd(), std:(".eliza"), std:(".elizadb")));
            }
            );
            it(std:("should use default path if no arguments or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                expect(result)->toBe(path->join(process->cwd(), std:(".eliza"), std:(".elizadb")));
            }
            );
            it(std:("should expand tilde paths"), [=]() mutable
            {
                auto result = resolvePgliteDir(std:("~/data/pglite"));
                expect(result)->toBe(path->join(process->cwd(), std:("data/pglite")));
            }
            );
        }
        );
    }
    );
}

MAIN
