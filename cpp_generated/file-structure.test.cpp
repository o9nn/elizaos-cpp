#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/project-starter/src/__tests__/file-structure.test.h"

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
    describe(std::string("Project Structure Validation"), [=]() mutable
    {
        shared rootDir = path->resolve(__dirname, std::string("../.."));
        describe(std::string("Directory Structure"), [=]() mutable
        {
            it(std::string("should have the expected directory structure"), [=]() mutable
            {
                expect(directoryExists(path->join(rootDir, std::string("src"))))->toBe(true);
                expect(directoryExists(path->join(rootDir, std::string("src"), std::string("__tests__"))))->toBe(true);
            }
            );
            it(std::string("should have a dist directory after building"), [=]() mutable
            {
                expect(directoryExists(path->join(rootDir, std::string("dist"))))->toBe(true);
            }
            );
        }
        );
        describe(std::string("Source Files"), [=]() mutable
        {
            it(std::string("should contain the required source files"), [=]() mutable
            {
                expect(fileExists(path->join(rootDir, std::string("src"), std::string("index.ts"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std::string("src"), std::string("plugin.ts"))))->toBe(true);
            }
            );
            it(std::string("should have properly structured main files"), [=]() mutable
            {
                auto indexContent = fs->readFileSync(path->join(rootDir, std::string("src"), std::string("index.ts")), std::string("utf8"));
                expect(indexContent)->toContain(std::string("character"));
                expect(indexContent)->toContain(std::string("plugin"));
                auto pluginContent = fs->readFileSync(path->join(rootDir, std::string("src"), std::string("plugin.ts")), std::string("utf8"));
                expect(pluginContent)->toContain(std::string("export default"));
                expect(pluginContent)->toContain(std::string("actions"));
            }
            );
        }
        );
        describe(std::string("Configuration Files"), [=]() mutable
        {
            it(std::string("should have the required configuration files"), [=]() mutable
            {
                expect(fileExists(path->join(rootDir, std::string("package.json"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std::string("tsconfig.json"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std::string("tsconfig.build.json"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std::string("tsup.config.ts"))))->toBe(true);
                expect(fileExists(path->join(rootDir, std::string("bunfig.toml"))))->toBe(true);
            }
            );
            it(std::string("should have the correct package.json configuration"), [=]() mutable
            {
                auto packageJson = JSON->parse(fs->readFileSync(path->join(rootDir, std::string("package.json")), std::string("utf8")));
                expect(packageJson["name"])->toBe(std::string("@elizaos/project-starter"));
                expect(packageJson["scripts"])->toHaveProperty(std::string("build"));
                expect(packageJson["scripts"])->toHaveProperty(std::string("test"));
                expect(packageJson["scripts"])->toHaveProperty(std::string("test:coverage"));
                expect(packageJson["dependencies"])->toHaveProperty(std::string("@elizaos/core"));
                expect(packageJson["devDependencies"])->toBeTruthy();
                expect(packageJson["devDependencies"])->toHaveProperty(std::string("tsup"));
            }
            );
            it(std::string("should have proper TypeScript configuration"), [=]() mutable
            {
                auto tsConfig = JSON->parse(fs->readFileSync(path->join(rootDir, std::string("tsconfig.json")), std::string("utf8")));
                expect(tsConfig)->toHaveProperty(std::string("compilerOptions"));
                expect(tsConfig["compilerOptions"])->toHaveProperty(std::string("target"));
                expect(tsConfig["compilerOptions"])->toHaveProperty(std::string("module"));
                expect(tsConfig)->toHaveProperty(std::string("include"));
            }
            );
        }
        );
        describe(std::string("Build Output"), [=]() mutable
        {
            it(std::string("should check for expected build output structure"), [=]() mutable
            {
                if (directoryExists(path->join(rootDir, std::string("dist")))) {
                    auto files = fs->readdirSync(path->join(rootDir, std::string("dist")));
                    expect(files->length)->toBeGreaterThan(0);
                    auto hasJsFiles = files->some([=](auto file) mutable
                    {
                        return file["endsWith"](std::string(".js"));
                    }
                    );
                    expect(hasJsFiles)->toBe(true);
                } else {
                    logger->warn(std::string("Dist directory not found, skipping build output tests"));
                }
            }
            );
            it(std::string("should verify the build process can be executed"), [=]() mutable
            {
                auto packageJson = JSON->parse(fs->readFileSync(path->join(rootDir, std::string("package.json")), std::string("utf8")));
                expect(packageJson["scripts"])->toHaveProperty(std::string("build"));
                auto tsupConfig = fs->readFileSync(path->join(rootDir, std::string("tsup.config.ts")), std::string("utf8"));
                expect(tsupConfig)->toContain(std::string("export default"));
                expect(tsupConfig)->toContain(std::string("entry"));
            }
            );
        }
        );
        describe(std::string("Documentation"), [=]() mutable
        {
            it(std::string("should have README files"), [=]() mutable
            {
                expect(fileExists(path->join(rootDir, std::string("README.md"))))->toBe(true);
            }
            );
            it(std::string("should have appropriate documentation content"), [=]() mutable
            {
                auto readmeContent = fs->readFileSync(path->join(rootDir, std::string("README.md")), std::string("utf8"));
                expect(readmeContent)->toContain(std::string("Project Starter"));
                expect(readmeContent)->toContain(std::string("Development"));
                expect(readmeContent)->toContain(std::string("Testing"));
            }
            );
        }
        );
    }
    );
}

MAIN
