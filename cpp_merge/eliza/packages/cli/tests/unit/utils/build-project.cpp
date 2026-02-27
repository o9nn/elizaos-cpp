#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/build-project.test.h"

object mockLogger = object{
    object::pair{std:("info"), mock()}, 
    object::pair{std:("success"), mock()}, 
    object::pair{std:("error"), mock()}, 
    object::pair{std:("warn"), mock()}, 
    object::pair{std:("debug"), mock()}
};
any mockExeca = mock();
any mockRunBunCommand = mock();
any mockExistsSync = mock();
any mockReadFileSync = mock();
any mockRm = mock();
any mockDetectDirectoryType = mock();

void Main(void)
{
    mock->module(std:("@elizaos/core"), [=]() mutable
    {
        return (object{
            object::pair{std:("logger"), mockLogger}
        });
    }
    );
    mock->module(std:("execa"), [=]() mutable
    {
        return (object{
            object::pair{std:("execa"), mockExeca}
        });
    }
    );
    mock->module(std:("../../../src/utils/run-bun"), [=]() mutable
    {
        return (object{
            object::pair{std:("runBunCommand"), mockRunBunCommand}
        });
    }
    );
    mock->module(std:("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std:("existsSync"), mockExistsSync}, 
            object::pair{std:("readFileSync"), mockReadFileSync}, 
            object::pair{std:("promises"), object{
                object::pair{std:("rm"), mockRm}
            }}
        });
    }
    );
    mock->module(std:("../../../src/utils/directory-detection"), [=]() mutable
    {
        return (object{
            object::pair{std:("detectDirectoryType"), mockDetectDirectoryType}
        });
    }
    );
    describe(std:("buildProject"), [=]() mutable
    {
        shared testProjectPath = std:("/test/project");
        shared testPluginPath = std:("/test/plugin");
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
                object::pair{std:("type"), std:("elizaos-project")}, 
                object::pair{std:("hasPackageJson"), true}, 
                object::pair{std:("hasElizaOSDependencies"), true}, 
                object::pair{std:("elizaPackageCount"), 1}
            });
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std:("name"), std:("test-project")}, 
                object::pair{std:("scripts"), object{
                    object::pair{std:("build"), std:("bun run build")}
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
        it(std:("should log correct messages and call runBunCommand for project build"), [=]() mutable
        {
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Building project in ") + testProjectPath + std:("..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Build completed successfully"));
            expect(mockRunBunCommand)->toHaveBeenCalledWith(array<string>{ std:("run"), std:("build") }, testProjectPath);
        }
        );
        it(std:("should log correct messages and call runBunCommand for plugin build"), [=]() mutable
        {
            std::async([=]() { buildProject(testPluginPath, true); });
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Building plugin in ") + testPluginPath + std:("..."));
            expect(mockLogger["info"])->toHaveBeenCalledWith(std:("Build completed successfully"));
            expect(mockRunBunCommand)->toHaveBeenCalledWith(array<string>{ std:("run"), std:("build") }, testPluginPath);
        }
        );
        it(std:("should clean dist directory before building"), [=]() mutable
        {
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                return OR((String(path)->includes(std:("dist"))), (!String(path)->includes(std:("tsconfig.json"))));
            }
            );
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockRm)->toHaveBeenCalledWith(expect->stringContaining(std:("dist")), object{
                object::pair{std:("recursive"), true}, 
                object::pair{std:("force"), true}
            });
        }
        );
        it(std:("should fallback to tsc when no build script exists"), [=]() mutable
        {
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std:("name"), std:("test-project")}
            }));
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                auto pathStr = String(path);
                if (pathStr->includes(std:("tsconfig.json"))) return true;
                return !pathStr->includes(std:("dist"));
            }
            );
            mockExeca->mockResolvedValue(object{
                object::pair{std:("exitCode"), 0}
            });
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockExeca)->toHaveBeenCalledWith(std:("bunx"), array<string>{ std:("tsc"), std:("--build") }, expect->objectContaining(object{
                object::pair{std:("cwd"), testProjectPath}, 
                object::pair{std:("stdio"), std:("inherit")}
            }));
        }
        );
        it(std:("should throw error when directory does not exist"), [=]() mutable
        {
            mockExistsSync->mockReturnValue(false);
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std:("Project directory ") + testProjectPath + std:(" does not exist.")); });
        }
        );
        it(std:("should throw error when package.json does not exist"), [=]() mutable
        {
            mockDetectDirectoryType->mockReturnValue(object{
                object::pair{std:("type"), std:("elizaos-project")}, 
                object::pair{std:("hasPackageJson"), false}, 
                object::pair{std:("hasElizaOSDependencies"), true}, 
                object::pair{std:("elizaPackageCount"), 1}
            });
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std:("Project directory ") + testProjectPath + std:(" does not have package.json.")); });
        }
        );
        it(std:("should handle build errors and log them correctly"), [=]() mutable
        {
            auto buildError = std::make_shared<Error>(std:("Build failed"));
            mockRunBunCommand->mockRejectedValue(buildError);
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std:("Failed to build using bun: Error: Build failed")); });
            expect(mockLogger["error"])->toHaveBeenCalledWith(std:("Failed to build project: Error: Failed to build using bun: Error: Build failed"));
        }
        );
        it(std:("should throw error when no build method can be determined"), [=]() mutable
        {
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std:("name"), std:("test-project")}
            }));
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                auto pathStr = String(path);
                return AND((!pathStr->includes(std:("tsconfig.json"))), (!pathStr->includes(std:("dist"))));
            }
            );
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std:("Could not determine how to build the project")); });
        }
        );
        it(std:("should warn when no build script is found"), [=]() mutable
        {
            mockReadFileSync->mockReturnValue(JSON->stringify(object{
                object::pair{std:("name"), std:("test-project")}
            }));
            mockExistsSync->mockImplementation([=](auto path) mutable
            {
                auto pathStr = String(path);
                if (pathStr->includes(std:("tsconfig.json"))) return true;
                return !pathStr->includes(std:("dist"));
            }
            );
            mockExeca->mockResolvedValue(object{
                object::pair{std:("exitCode"), 0}
            });
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockLogger["warn"])->toHaveBeenCalledWith(expect->stringContaining(std:("No build script found")));
        }
        );
    }
    );
}

MAIN
