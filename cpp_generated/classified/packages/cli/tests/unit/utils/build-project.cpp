#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/tests/unit/utils/build-project.test.h"

object mockLogger = object{
    object::pair{std::string("info"), mock()}, 
    object::pair{std::string("success"), mock()}, 
    object::pair{std::string("error"), mock()}, 
    object::pair{std::string("warn"), mock()}, 
    object::pair{std::string("debug"), mock()}
};
any mockBunExec = mock();
any mockRunBunWithSpinner = mock();
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
    mock->module(std::string("../../../src/utils/bun-exec"), [=]() mutable
    {
        return (object{
            object::pair{std::string("bunExec"), mockBunExec}
        });
    }
    );
    mock->module(std::string("../../../src/utils/spinner-utils"), [=]() mutable
    {
        return (object{
            object::pair{std::string("runBunWithSpinner"), mockRunBunWithSpinner}
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
            mockBunExec->mockClear();
            mockRunBunWithSpinner->mockClear();
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
            mockRunBunWithSpinner->mockResolvedValue(object{
                object::pair{std::string("success"), true}
            });
            mockRm->mockResolvedValue(undefined);
        }
        );
        afterEach([=]() mutable
        {
        }
        );
        it(std::string("should log correct messages and call runBunWithSpinner for project build"), [=]() mutable
        {
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockRunBunWithSpinner)->toHaveBeenCalledWith(array<string>{ std::string("run"), std::string("build") }, testProjectPath, expect->objectContaining(object{
                object::pair{std::string("spinnerText"), std::string("Building project...")}, 
                object::pair{std::string("successText"), expect->stringContaining(std::string("Project built successfully"))}, 
                object::pair{std::string("errorText"), std::string("Failed to build project")}
            }));
        }
        );
        it(std::string("should log correct messages and call runBunWithSpinner for plugin build"), [=]() mutable
        {
            std::async([=]() { buildProject(testPluginPath, true); });
            expect(mockRunBunWithSpinner)->toHaveBeenCalledWith(array<string>{ std::string("run"), std::string("build") }, testPluginPath, expect->objectContaining(object{
                object::pair{std::string("spinnerText"), std::string("Building plugin...")}, 
                object::pair{std::string("successText"), expect->stringContaining(std::string("Plugin built successfully"))}, 
                object::pair{std::string("errorText"), std::string("Failed to build plugin")}
            }));
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
            mockBunExec->mockResolvedValue(object{
                object::pair{std::string("success"), true}, 
                object::pair{std::string("stdout"), string_empty}, 
                object::pair{std::string("stderr"), string_empty}, 
                object::pair{std::string("exitCode"), 0}
            });
            std::async([=]() { buildProject(testProjectPath); });
            expect(mockBunExec)->toHaveBeenCalledWith(std::string("bunx"), array<string>{ std::string("tsc"), std::string("--build") }, expect->objectContaining(object{
                object::pair{std::string("cwd"), testProjectPath}
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
            mockRunBunWithSpinner->mockResolvedValue(object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("error"), buildError}
            });
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std::string("Build failed")); });
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
        it(std::string("should handle tsc build failure"), [=]() mutable
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
            mockBunExec->mockResolvedValue(object{
                object::pair{std::string("success"), false}, 
                object::pair{std::string("stdout"), string_empty}, 
                object::pair{std::string("stderr"), std::string("tsc error")}, 
                object::pair{std::string("exitCode"), 1}
            });
            std::async([=]() { expect(buildProject(testProjectPath))->rejects->toThrow(std::string("bunx tsc build failed: tsc error")); });
        }
        );
    }
    );
}

MAIN
