#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/plugin-sql/src/__tests__/unit/utils.test.h"

void Main(void)
{
    describe(std::string("Utils"), [=]() mutable
    {
        describe(std::string("expandTildePath"), [=]() mutable
        {
            it(std::string("should expand paths starting with ~"), [=]() mutable
            {
                auto result = expandTildePath(std::string("~/test/path"));
                expect(result)->toBe(path->join(process->cwd(), std::string("test/path")));
            }
            );
            it(std::string("should return unchanged paths not starting with ~"), [=]() mutable
            {
                auto result = expandTildePath(std::string("/absolute/path"));
                expect(result)->toBe(std::string("/absolute/path"));
            }
            );
            it(std::string("should handle empty strings"), [=]() mutable
            {
                auto result = expandTildePath(string_empty);
                expect(result)->toBe(string_empty);
            }
            );
            it(std::string("should handle just tilde"), [=]() mutable
            {
                auto result = expandTildePath(std::string("~"));
                expect(result)->toBe(process->cwd());
            }
            );
        }
        );
        describe(std::string("resolveEnvFile"), [=]() mutable
        {
            it(std::string("should find .env in current directory if it exists"), [=]() mutable
            {
                auto result = resolveEnvFile();
                expect(result)->toMatch((new RegExp(std::string("\.env"))));
            }
            );
            it(std::string("should return .env path even if not found"), [=]() mutable
            {
                auto testDir = std::string("/some/nonexistent/path");
                auto result = resolveEnvFile(testDir);
                expect(result)->toBe(path->join(testDir, std::string(".env")));
            }
            );
        }
        );
        describe(std::string("resolvePgliteDir"), [=]() mutable
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
            it(std::string("should prioritize dir argument"), [=]() mutable
            {
                auto result = resolvePgliteDir(std::string("/custom/dir"));
                expect(result)->toBe(std::string("/custom/dir"));
            }
            );
            it(std::string("should use PGLITE_DATA_DIR env var if no dir provided"), [=]() mutable
            {
                process->env->PGLITE_DATA_DIR = std::string("/env/pglite/dir");
                auto result = resolvePgliteDir();
                expect(result)->toBe(std::string("/env/pglite/dir"));
            }
            );
            it(std::string("should use default .eliza/.elizadb dir if no dir or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                auto projectRoot = path->resolve(process->cwd(), std::string(".."), std::string(".."));
                expect(result)->toBe(path->join(projectRoot, std::string(".elizadb")));
            }
            );
            it(std::string("should use default path if no arguments or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                auto projectRoot = path->resolve(process->cwd(), std::string(".."), std::string(".."));
                expect(result)->toBe(path->join(projectRoot, std::string(".elizadb")));
            }
            );
            it(std::string("should expand tilde paths"), [=]() mutable
            {
                auto result = resolvePgliteDir(std::string("~/data/pglite"));
                expect(result)->toBe(path->join(process->cwd(), std::string("data/pglite")));
            }
            );
        }
        );
    }
    );
}

MAIN
