#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/env.test.h"

void Main(void)
{
    describe(std:("Environment Setup"), [=]() mutable
    {
        it(std:("should verify configuration files exist"), [=]() mutable
        {
            auto requiredFiles = array<string>{ std:("package.json"), std:("tsconfig.json"), std:("tsconfig.build.json"), std:("tsup.config.ts"), std:("bunfig.toml") };
            for (auto& file : requiredFiles)
            {
                auto filePath = path->join(process->cwd(), file);
                expect(fs->existsSync(filePath))->toBe(true);
            }
        }
        );
        it(std:("should have proper src directory structure"), [=]() mutable
        {
            auto srcDir = path->join(process->cwd(), std:("src"));
            expect(fs->existsSync(srcDir))->toBe(true);
            auto requiredSrcFiles = array<string>{ std:("index.ts"), std:("plugin.ts") };
            for (auto& file : requiredSrcFiles)
            {
                auto filePath = path->join(srcDir, file);
                expect(fs->existsSync(filePath))->toBe(true);
            }
        }
        );
        it(std:("should have a valid package.json with required fields"), [=]() mutable
        {
            auto packageJsonPath = path->join(process->cwd(), std:("package.json"));
            expect(fs->existsSync(packageJsonPath))->toBe(true);
            auto packageJson = JSON->parse(fs->readFileSync(packageJsonPath, std:("utf8")));
            expect(packageJson)->toHaveProperty(std:("name"), std:("@elizaos/project-starter"));
            expect(packageJson)->toHaveProperty(std:("version"));
            expect(packageJson)->toHaveProperty(std:("type"), std:("module"));
            expect(packageJson)->toHaveProperty(std:("main"));
            expect(packageJson)->toHaveProperty(std:("module"));
            expect(packageJson)->toHaveProperty(std:("types"));
            expect(packageJson)->toHaveProperty(std:("dependencies"));
            expect(packageJson)->toHaveProperty(std:("devDependencies"));
            expect(packageJson)->toHaveProperty(std:("scripts"));
            expect(packageJson["dependencies"])->toHaveProperty(std:("@elizaos/core"));
            expect(packageJson["scripts"])->toHaveProperty(std:("build"));
            expect(packageJson["scripts"])->toHaveProperty(std:("test"));
        }
        );
        it(std:("should have a valid tsconfig.json with required configuration"), [=]() mutable
        {
            auto tsconfigPath = path->join(process->cwd(), std:("tsconfig.json"));
            expect(fs->existsSync(tsconfigPath))->toBe(true);
            auto tsconfig = JSON->parse(fs->readFileSync(tsconfigPath, std:("utf8")));
            expect(tsconfig)->toHaveProperty(std:("compilerOptions"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std:("target"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std:("module"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std:("moduleResolution"));
            expect(tsconfig["compilerOptions"])->toHaveProperty(std:("esModuleInterop"));
        }
        );
        it(std:("should have a valid tsup.config.ts for building"), [=]() mutable
        {
            auto tsupConfigPath = path->join(process->cwd(), std:("tsup.config.ts"));
            expect(fs->existsSync(tsupConfigPath))->toBe(true);
            auto tsupConfig = fs->readFileSync(tsupConfigPath, std:("utf8"));
            expect(tsupConfig)->toContain(std:("defineConfig"));
            expect(tsupConfig)->toContain(std:("entry:"));
            expect(tsupConfig)->toContain(std:("src/index.ts"));
        }
        );
        it(std:("should have a valid README.md file"), [=]() mutable
        {
            auto readmePath = path->join(process->cwd(), std:("README.md"));
            expect(fs->existsSync(readmePath))->toBe(true);
            auto readme = fs->readFileSync(readmePath, std:("utf8"));
            expect(readme)->toContain(std:("# Project Starter"));
        }
        );
    }
    );
}

MAIN
