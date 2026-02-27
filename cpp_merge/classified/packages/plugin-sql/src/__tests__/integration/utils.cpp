#include "utils.test.h"

void Main(void)
{
    describe(std:("Utils Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<NodeJS::ProcessEnv>> originalEnv;
        shared<std::function<string()>> originalCwd;
        shared<string> tempDir;
        beforeEach([=]() mutable
        {
            originalEnv = utils::assign(object{
            }, process->env);
            originalCwd = process->cwd;
            tempDir = fs->mkdtempSync(path->join(os->tmpdir(), std:("utils-test-")));
            process->cwd = [=]() mutable
            {
                return tempDir;
            };
        }
        );
        afterEach([=]() mutable
        {
            process->env = originalEnv;
            process->cwd = originalCwd;
            if (fs->existsSync(tempDir)) {
                fs->rmSync(tempDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
        }
        );
        describe(std:("expandTildePath"), [=]() mutable
        {
            it(std:("should expand ~ to current working directory"), [=]() mutable
            {
                auto result = expandTildePath(std:("~/test/path"));
                expect(result)->toBe(path->join(tempDir, std:("test/path")));
            }
            );
            it(std:("should not change paths without ~"), [=]() mutable
            {
                auto absolutePath = std:("/absolute/path");
                expect(expandTildePath(absolutePath))->toBe(absolutePath);
                auto relativePath = std:("relative/path");
                expect(expandTildePath(relativePath))->toBe(relativePath);
            }
            );
            it(std:("should handle just ~ alone"), [=]() mutable
            {
                auto result = expandTildePath(std:("~"));
                expect(result)->toBe(tempDir);
            }
            );
            it(std:("should handle empty string"), [=]() mutable
            {
                expect(expandTildePath(string_empty))->toBe(string_empty);
            }
            );
        }
        );
        describe(std:("resolveEnvFile"), [=]() mutable
        {
            it(std:("should find .env in current directory"), [=]() mutable
            {
                fs->writeFileSync(path->join(tempDir, std:(".env")), std:("TEST=true"));
                auto result = resolveEnvFile(tempDir);
                expect(result)->toBe(path->join(tempDir, std:(".env")));
            }
            );
            it(std:("should traverse up directories to find .env"), [=]() mutable
            {
                auto subDir = path->join(tempDir, std:("sub"), std:("nested"));
                fs->mkdirSync(subDir, object{
                    object::pair{std:("recursive"), true}
                });
                fs->writeFileSync(path->join(tempDir, std:(".env")), std:("TEST=true"));
                auto result = resolveEnvFile(subDir);
                expect(result)->toBe(path->join(tempDir, std:(".env")));
            }
            );
            it(std:("should return .env path in start directory if not found"), [=]() mutable
            {
                auto subDir = path->join(tempDir, std:("sub"));
                fs->mkdirSync(subDir, object{
                    object::pair{std:("recursive"), true}
                });
                auto result = resolveEnvFile(subDir);
                expect(result)->toBe(path->join(subDir, std:(".env")));
            }
            );
            it(std:("should use current working directory if no startDir provided"), [=]() mutable
            {
                auto result = resolveEnvFile();
                expect(result)->toBe(path->join(tempDir, std:(".env")));
            }
            );
        }
        );
        describe(std:("resolvePgliteDir"), [=]() mutable
        {
            it(std:("should use provided dir argument"), [=]() mutable
            {
                auto customDir = std:("/custom/dir");
                auto result = resolvePgliteDir(customDir);
                expect(result)->toBe(customDir);
            }
            );
            it(std:("should use PGLITE_DATA_DIR environment variable"), [=]() mutable
            {
                auto envDir = std:("/env/dir");
                process->env->PGLITE_DATA_DIR = envDir;
                auto result = resolvePgliteDir();
                expect(result)->toBe(envDir);
            }
            );
            it(std:("should use fallback dir when no dir or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto fallbackDir = std:("/fallback/dir");
                auto result = resolvePgliteDir(undefined, fallbackDir);
                expect(result)->toBe(fallbackDir);
            }
            );
            it(std:("should use default path when no arguments or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                expect(result)->toBe(path->join(tempDir, std:(".eliza"), std:(".elizadb")));
            }
            );
            it(std:("should load .env file if it exists"), [=]() mutable
            {
                fs->writeFileSync(path->join(tempDir, std:(".env")), std:("PGLITE_DATA_DIR=/from/env/file"));
                process->env->PGLITE_DATA_DIR = std:("/from/env/file");
                auto result = resolvePgliteDir();
                expect(process->env->PGLITE_DATA_DIR)->toBe(as<any>(std:("/from/env/file")));
                expect(result)->toBe(as<any>(std:("/from/env/file")));
                process->env.Delete("PGLITE_DATA_DIR");
            }
            );
            it(std:("should expand tilde paths"), [=]() mutable
            {
                auto result = resolvePgliteDir(std:("~/data/pglite"));
                expect(result)->toBe(path->join(tempDir, std:("data/pglite")));
            }
            );
            it(std:("should migrate legacy path to new location"), [=]() mutable
            {
                auto legacyPath = path->join(tempDir, std:(".elizadb"));
                auto result = resolvePgliteDir(legacyPath);
                expect(result)->toBe(path->join(tempDir, std:(".eliza"), std:(".elizadb")));
                expect(process->env->PGLITE_DATA_DIR)->toBe(path->join(tempDir, std:(".eliza"), std:(".elizadb")));
            }
            );
        }
        );
    }
    );
}

MAIN
