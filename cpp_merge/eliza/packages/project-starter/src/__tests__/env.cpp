#include "env.test.h"
#include <string>

void Main(void)
{
    describe(std::string("Environment Setup"), [=]() mutable
    {
        it(std::string("should verify configuration files exist"), [=]() mutable
        {
            auto requiredFiles = array<string>{ std::string("package.json"), std::string("tsconfig.json"), std::string("tsconfig.build.json"), std::string("tsup.config.ts"), std::string("bunfig.toml") };
            for (auto& file : requiredFiles)
            {
                auto filePath = path->join(process->cwd(), file);
                expect(fs->existsSync(filePath))->toBe(true);
            }
        }
        );
        it(std::string("should have proper src directory structure"), [=]() mutable
        {
            auto srcDir = path->join(process->cwd(), std::string("src"));
            expect(fs->existsSync(srcDir))->toBe(true);
            auto requiredSrcFiles = array<string>{ std::string("index.ts"), std::string("plugin.ts") };
            for (auto& file : requiredSrcFiles)
            {
                auto filePath = path->join(srcDir, file);
                expect(fs->existsSync(filePath))->toBe(true);
            }
        }
        );
        it(std::string("should have a valid package.json with required fields"), [=]() mutable
        {
            auto packageJsonPath = path->join(process->cwd(), std::string("package.json"));
            expect(fs->existsSync(packageJsonPath))->toBe(true);
            auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std::string("utf8")));
            expect(packageJson)->toHaveProperty(std::string("name"), std::string("@elizaos/project-starter"));
            expect(packageJson)->toHaveProperty(std::string("version"));
            expect(packageJson)->toHaveProperty(std::string("type"), std::string("module"));
            expect(packageJson)->toHaveProperty(std::string("main"));
            expect(packageJson)->toHaveProperty(std::string("module"));
            expect(packageJson)->toHaveProperty(std::string("types"));
            expect(packageJson)->toHaveProperty(std::string("dependencies"));
            expect(packageJson)->toHaveProperty(std::string("devDependencies"));
            expect(packageJson)->toHaveProperty(std::string("scripts"));
            expect(packageJson["dependencies"])->toHaveProperty(std::string("@elizaos/core"));
            expect(packageJson["scripts"])->toHaveProperty(std::string("build"));
            expect(packageJson["scripts"])->toHaveProperty(std::string("test"));
        }
        );
        it(std::string("should have a valid tsconfig.json with required configuration"), [=]() mutable
        {
            auto tsconfigPath = path->join(process->cwd(), std::string("tsconfig.json"));
            expect(fs->existsSync(tsconfigPath))->toBe(true);
            auto tsconfig = JSON->parse(fs->readFileSync(tsconfigPath, std::string("utf8")));
            expect(tsconfig)->toHaveProperty(std::string("compilerOptions"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std::string("target"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std::string("module"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std::string("moduleResolution"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std::string("esModuleInterop"));
        }
        );
        it(std::string("should have a valid tsup.config.ts for building"), [=]() mutable
        {
            auto tsupConfigPath = path->join(process->cwd(), std::string("tsup.config.ts"));
            expect(fs->existsSync(tsupConfigPath))->toBe(true);
            auto tsupConfig = fs->readFileSync(tsupConfigPath, std::string("utf8"));
            expect(tsupConfig)->toContain(std::string("defineConfig"));
            expect(tsupConfig)->toContain(std::string("entry:"));
            expect(tsupConfig)->toContain(std::string("src/index.ts"));
        }
        );
        it(std::string("should have a valid README.md file"), [=]() mutable
        {
            auto readmePath = path->join(process->cwd(), std::string("README.md"));
            expect(fs->existsSync(readmePath))->toBe(true);
            auto readme = fs->readFileSync(readmePath, std::string("utf8"));
            expect(readme)->toContain(std::string("# Project Starter"));
        }
        );
    }
    );
}

MAIN
