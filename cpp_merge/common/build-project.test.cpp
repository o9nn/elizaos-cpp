#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/build-project.test.h"

object mockLogger = object{
    object::pair{std::string("info"), mock()}, 
    object::pair{std::string("success"), mock()}, 
    object::pair{std::string("error"), mock()}, 
    object::pair{std::string("warn"), mock()}, 
    object::pair{std::string("debug"), mock()}
};
any mockExeca = mock();
any mockRunBunCommand = mock();
any mockExistsSync = mock();
any mockReadFileSync = mock();
any mockRm = mock();
any mockDetectDirectoryType = mock();

void Main(void)
{
    mock->module(std::string("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std::string("logger"), mockLogger}
        });
    }
    );
    mock->module(std::string("execa"), [=]() mutable
    {
        return (object{
            object::pair{std::string("execa"), mockExeca}
        });
    }
    );
    mock->module(std::string("../../../src/utils/run-bun"), [=]() mutable
    {
        return (object{
            object::pair{std::string("runBunCommand"), mockRunBunCommand}
        });
    }
    );
    mock->module(std::string("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("existsSync"), mockExistsSync}, 
            object::pair{std::string("readFileSync"), mockReadFileSync}, 
            object::pair{std::string("promises"), object{
                object::pair{std::string("rm"), mockRm}
            }}
        });
    }
    );
    mock->module(std::string("../../../src/utils/directory-detection"), [=]() mutable
    {
        return (object{
            object::pair{std::string("detectDirectoryType"), mockDetectDirectoryType}
        });
    }
    );
    describe(std::string("buildProject"), [=]() mutable
    {
        shared testProjectPath = std::string("/test/project");
        shared testPluginPath = std::string("/test/plugin");
        beforeEach([=]() mutable
        {
            mockLogger["info"]->mockClear();
            mockLogger["success"]->mockClear();
            mockLogger["error"]->mockClear();
            mockLogger["warn"]->mockClear();
            mockLogger["debug"]->mockClear();
            mockExeca->mockClear();
            mockRunBunCommand->mockClear();
            mockExistsSync->mockClear();
            mockReadFileSync->mockClear();
            mockRm->mockClear();
            mockDetectDirectoryType->mockClear();
            mockExistsSync->mockReturnValue(true);
            mockDetectDirectoryType->mockReturnValue(object{
                object::pair{std::string("type"), std::string("elizaos-project")}, 
                object::pair{std::string("hasPackageJson"), true}, 
                object::pair{std::string("hasElizaOSDependencies"), true}, 
                object::pair{std::string("elizaPackageCount"), 1}
            });
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std::string("name"), std::string("test-project")}, 
                object::pair{std::string("scripts"), object{
                    object::pair{std::string("build"), std::string("bun run build")}
                }}
            }));
            mockRunBunCommand->mockResolvedValue(undefined);
            mockRm->mockResolvedValue(undefined);
        }
        );
        afterEach([=]() mutable
        {
        }
        );
        it(std::string("should log correct messages and call runBunCommand for project build"), [=]() mutable
        {
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Building project in ") + testProjectPath + std::string("..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Build completed successfully"));
            expect(mockRunBunCommand)->toHaveBeenCalledWith(array<string>{ std::string("run"), std::string("build") }, testProjectPath);
        }
        );
        it(std::string("should log correct messages and call runBunCommand for plugin build"), [=]() mutable
        {
            std::async([=]() { buildProject(testPluginPath, true); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Building plugin in ") + testPluginPath + std::string("..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std::string("Build completed successfully"));
            expect(mockRunBunCommand)->toHaveBeenCalledWith(array<string>{ std::string("run"), std::string("build") }, testPluginPath);
        }
        );
        it(std::string("should clean dist directory before building"), [=]() mutable
        {
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                return OR((String(path)->includes(std::string("dist"))), (!String(path)->includes(std::string("tsconfig.json"))));
            }
            );
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockRm)->toHaveBeenCalledWith(expect->stringContaining(std::string("dist")), object{
                object::pair{std::string("recursive"), true}, 
                object::pair{std::string("force"), true}
            });
        }
        );
        it(std::string("should fallback to tsc when no build script exists"), [=]() mutable
        {
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std::string("name"), std::string("test-project")}
            }));
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                auto pathStr = String(path);
                if (pathStr->includes(std::string("tsconfig.json"))) return true;
                return !pathStr->includes(std::string("dist"));
            }
            );
            mockExeca->mockResolvedValue(object{
                object::pair{std::string("exitCode"), 0}
            });
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockExeca)->toHaveBeenCalledWith(std::string("bunx"), array<string>{ std::string("tsc"), std::string("--build") }, expect->objectContaining(object{
                object::pair{std::string("cwd"), testProjectPath}, 
                object::pair{std::string("stdio"), std::string("inherit")}
            }));
        }
        );
        it(std::string("should throw error when directory does not exist"), [=]() mutable
        {
            mockExistsSync->mockReturnValue(false);
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std::string("Project directory ") + testProjectPath + std::string(" does not exist.")); });
        }
        );
        it(std::string("should throw error when package.json does not exist"), [=]() mutable
        {
            mockDetectDirectoryType->mockReturnValue(object{
                object::pair{std::string("type"), std::string("elizaos-project")}, 
                object::pair{std::string("hasPackageJson"), false}, 
                object::pair{std::string("hasElizaOSDependencies"), true}, 
                object::pair{std::string("elizaPackageCount"), 1}
            });
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std::string("Project directory ") + testProjectPath + std::string(" does not have package.json.")); });
        }
        );
        it(std::string("should handle build errors and log them correctly"), [=]() mutable
        {
            auto buildError = std::make_shared<Error>(std::string("Build failed"));
            mockRunBunCommand->mockRejectedValue(buildError);
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std::string("Failed to build using bun: Error: Build failed")); });
            expect(mockLogger["error"])->toHaveBeenCalledWith(std::string("Failed to build project: Error: Failed to build using bun: Error: Build failed"));
        }
        );
        it(std::string("should throw error when no build method can be determined"), [=]() mutable
        {
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std::string("name"), std::string("test-project")}
            }));
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                auto pathStr = String(path);
                return AND((!pathStr->includes(std::string("tsconfig.json"))), (!pathStr->includes(std::string("dist"))));
            }
            );
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std::string("Could not determine how to build the project")); });
        }
        );
        it(std::string("should warn when no build script is found"), [=]() mutable
        {
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std::string("name"), std::string("test-project")}
            }));
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                auto pathStr = String(path);
                if (pathStr->includes(std::string("tsconfig.json"))) return true;
                return !pathStr->includes(std::string("dist"));
            }
            );
            mockExeca->mockResolvedValue(object{
                object::pair{std::string("exitCode"), 0}
            });
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockLogger["warn"])->toHaveBeenCalledWith(expect->stringContaining(std::string("No build script found")));
        }
        );
    }
    );
}

MAIN
