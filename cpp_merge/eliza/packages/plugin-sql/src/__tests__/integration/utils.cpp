#include "utils.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Utils Integration Tests"), [=]() mutable
    {
        shared<std::shared_ptr<NodeJS::ProcessEnv>> originalEnv;
        shared<std::function<string()>> originalCwd;
        shared<string> tempDir;
        beforeEach([=]() mutable
        {
            originalEnv = utils::assign(object{
            }, process->env);
            originalCwd = process->cwd;
            tempDir = fs->mkdtempSync(path->join(os->tmpdir(), std::string("utils-test-")));
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
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
        }
        );
        describe(std::string("expandTildePath"), [=]() mutable
        {
            it(std::string("should expand ~ to current working directory"), [=]() mutable
            {
                auto result = expandTildePath(std::string("~/test/path"));
                expect(result)->toBe(path->join(tempDir, std::string("test/path")));
            }
            );
            it(std::string("should not change paths without ~"), [=]() mutable
            {
                auto absolutePath = std::string("/absolute/path");
                expect(expandTildePath(absolutePath))->toBe(absolutePath);
                auto relativePath = std::string("relative/path");
                expect(expandTildePath(relativePath))->toBe(relativePath);
            }
            );
            it(std::string("should handle just ~ alone"), [=]() mutable
            {
                auto result = expandTildePath(std::string("~"));
                expect(result)->toBe(tempDir);
            }
            );
            it(std::string("should handle empty string"), [=]() mutable
            {
                expect(expandTildePath(string_empty))->toBe(string_empty);
            }
            );
        }
        );
        describe(std::string("resolveEnvFile"), [=]() mutable
        {
            it(std::string("should find .env in current directory"), [=]() mutable
            {
                fs->writeFileSync(path->join(tempDir, std::string(".env")), std::string("TEST=true"));
                auto result = resolveEnvFile(tempDir);
                expect(result)->toBe(path->join(tempDir, std::string(".env")));
            }
            );
            it(std::string("should traverse up directories to find .env"), [=]() mutable
            {
                auto subDir = path->join(tempDir, std::string("sub"), std::string("nested"));
                fs->mkdirSync(subDir, object{
                    object::pair{std::string("recursive"), true}
                });
                fs->writeFileSync(path->join(tempDir, std::string(".env")), std::string("TEST=true"));
                auto result = resolveEnvFile(subDir);
                expect(result)->toBe(path->join(tempDir, std::string(".env")));
            }
            );
            it(std::string("should return .env path in start directory if not found"), [=]() mutable
            {
                auto subDir = path->join(tempDir, std::string("sub"));
                fs->mkdirSync(subDir, object{
                    object::pair{std::string("recursive"), true}
                });
                auto result = resolveEnvFile(subDir);
                expect(result)->toBe(path->join(subDir, std::string(".env")));
            }
            );
            it(std::string("should use current working directory if no startDir provided"), [=]() mutable
            {
                auto result = resolveEnvFile();
                expect(result)->toBe(path->join(tempDir, std::string(".env")));
            }
            );
        }
        );
        describe(std::string("resolvePgliteDir"), [=]() mutable
        {
            it(std::string("should use provided dir argument"), [=]() mutable
            {
                auto customDir = std::string("/custom/dir");
                auto result = resolvePgliteDir(customDir);
                expect(result)->toBe(customDir);
            }
            );
            it(std::string("should use PGLITE_DATA_DIR environment variable"), [=]() mutable
            {
                auto envDir = std::string("/env/dir");
                process->env->PGLITE_DATA_DIR = envDir;
                auto result = resolvePgliteDir();
                expect(result)->toBe(envDir);
            }
            );
            it(std::string("should use fallback dir when no dir or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto fallbackDir = std::string("/fallback/dir");
                auto result = resolvePgliteDir(std::nullopt, fallbackDir);
                expect(result)->toBe(fallbackDir);
            }
            );
            it(std::string("should use default path when no arguments or env var"), [=]() mutable
            {
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                expect(result)->toBe(path->join(tempDir, std::string(".eliza"), std::string(".elizadb")));
            }
            );
            it(std::string("should load .env file if it exists"), [=]() mutable
            {
                fs->writeFileSync(path->join(tempDir, std::string(".env")), std::string("PGLITE_DATA_DIR=/from/env/file"));
                process->env.Delete("PGLITE_DATA_DIR");
                auto result = resolvePgliteDir();
                expect(process->env->PGLITE_DATA_DIR)->toBe(as<any>(std::string("/from/env/file")));
                expect(result)->toBe(as<any>(std::string("/from/env/file")));
            }
            );
            it(std::string("should expand tilde paths"), [=]() mutable
            {
                auto result = resolvePgliteDir(std::string("~/data/pglite"));
                expect(result)->toBe(path->join(tempDir, std::string("data/pglite")));
            }
            );
            it(std::string("should migrate legacy path to new location"), [=]() mutable
            {
                auto legacyPath = path->join(tempDir, std::string(".elizadb"));
                auto result = resolvePgliteDir(legacyPath);
                expect(result)->toBe(path->join(tempDir, std::string(".eliza"), std::string(".elizadb")));
                expect(process->env->PGLITE_DATA_DIR)->toBe(path->join(tempDir, std::string(".eliza"), std::string(".elizadb")));
            }
            );
        }
        );
    }
    );
}

MAIN
