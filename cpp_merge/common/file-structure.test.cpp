#include "file-structure.test.h"

boolean fileExists(string filePath)
{
    return fs->existsSync(filePath);
};


boolean directoryExists(string dirPath)
{
    return AND((fs->existsSync(dirPath)), (fs->statSync(dirPath)->isDirectory()));
};



void Main(void)
{
    describe(std:("Project Structure Validation"), [=]() mutable
    {
        shared rootDir = path->resolve(__dirname, std:(".."));
        describe(std:("Directory Structure"), [=]() mutable
        {
            it(std:("should have the expected directory structure"), [=]() mutable
            {
                expect(directoryExists(path->join(rootDir, std:("src"))))->toBe(true);
                expect(directoryExists(path->join(rootDir, std:("__tests__"))))->toBe(true);
            }
            );
            it(std:("should have a dist directory after building"), [=]() mutable
            {
                expect(directoryExists(path->join(rootDir, std:("dist"))))->toBe(true);
            }
            );
        }
        );
        describe(std:("Source Files"), [=]() mutable
        {
            it(std:("should contain the required source files"), [=]() mutable
            {
                expect(fileExists(path->join(rootDir, std:("src"), std:("index.ts"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std:("src"), std:("plugin.ts"))))->toBe(true);
            }
            );
            it(std:("should have properly structured main files"), [=]() mutable
            {
                auto indexContent = fs->readFileSync(path->join(rootDir, std:("src"), std:("index.ts")), std:("utf8"));
                expect(indexContent)->toContain(std:("character"));
                expect(indexContent)->toContain(std:("plugin"));
                auto pluginContent = fs->readFileSync(path->join(rootDir, std:("src"), std:("plugin.ts")), std:("utf8"));
                expect(pluginContent)->toContain(std:("default"));
                expect(pluginContent)->toContain(std:("actions"));
            }
            );
        }
        );
        describe(std:("Configuration Files"), [=]() mutable
        {
            it(std:("should have the required configuration files"), [=]() mutable
            {
                expect(fileExists(path->join(rootDir, std:("package.json"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std:("tsconfig.json"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std:("tsconfig.build.json"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std:("tsup.config.ts"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std:("bunfig.toml"))))->toBe(true);
            }
            );
            it(std:("should have the correct package.json configuration"), [=]() mutable
            {
                auto packageJson = JSON->parse(fs->readFileSync(path->join(rootDir, std:("package.json")), std:("utf8")));
                expect(packageJson["name"])->toBe(std:("@elizaos/project-tee-starter"));
                expect(packageJson["scripts"])->toHaveProperty(std:("build"));
                expect(packageJson["scripts"])->toHaveProperty(std:("test"));
                expect(packageJson["scripts"])->toHaveProperty(std:("test:coverage"));
                expect(packageJson["dependencies"])->toHaveProperty(std:("@elizaos/core"));
                expect(packageJson["devDependencies"])->toBeTruthy();
                expect(packageJson["devDependencies"])->toHaveProperty(std:("tsup"));
            }
            );
            it(std:("should have proper TypeScript configuration"), [=]() mutable
            {
                auto tsConfig = JSON->parse(fs->readFileSync(path->join(rootDir, std:("tsconfig.json")), std:("utf8")));
                expect(tsConfig)->toHaveProperty(std:("compilerOptions"));
                expect(tsConfig["compilerOptions"])->toHaveProperty(std:("target"));
                expect(tsConfig["compilerOptions"])->toHaveProperty(std:("module"));
                expect(tsConfig)->toHaveProperty(std:("include"));
            }
            );
        }
        );
        describe(std:("Build Output"), [=]() mutable
        {
            it(std:("should check for expected build output structure"), [=]() mutable
            {
                if (directoryExists(path->join(rootDir, std:("dist")))) {
                    auto files = fs->readdirSync(path->join(rootDir, std:("dist")));
                    expect(files->length)->toBeGreaterThan(0);
                    auto hasJsFiles = files->some([=](auto file) mutable
                    {
                        return file["endsWith"](std:(".js"));
                    }
                    );
                    expect(hasJsFiles)->toBe(true);
                } else {
                    logger->warn(std:("Dist directory not found, skipping build output tests"));
                }
            }
            );
            it(std:("should verify the build process can be executed"), [=]() mutable
            {
                auto packageJson = JSON->parse(fs->readFileSync(path->join(rootDir, std:("package.json")), std:("utf8")));
                expect(packageJson["scripts"])->toHaveProperty(std:("build"));
                auto tsupConfig = fs->readFileSync(path->join(rootDir, std:("tsup.config.ts")), std:("utf8"));
                expect(tsupConfig)->toContain(std:("default"));
                expect(tsupConfig)->toContain(std:("entry"));
            }
            );
        }
        );
        describe(std:("Documentation"), [=]() mutable
        {
            it(std:("should have README files"), [=]() mutable
            {
                expect(fileExists(path->join(rootDir, std:("README.md"))))->toBe(true);
            }
            );
            it(std:("should have appropriate documentation content"), [=]() mutable
            {
                auto readmeContent = fs->readFileSync(path->join(rootDir, std:("README.md")), std:("utf8"));
                expect(readmeContent)->toContain(std:("Project Starter"));
                expect(readmeContent)->toContain(std:("Development"));
                expect(readmeContent)->toContain(std:("Testing"));
            }
            );
        }
        );
    }
    );
}

MAIN
