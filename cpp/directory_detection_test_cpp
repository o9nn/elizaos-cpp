#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/tests/unit/utils/directory-detection.test.h"

object TestFixtures = object{
    object::pair{std::string("packageJson"), object{
        object::pair{std::string("elizaProject"), object{
            object::pair{std::string("name"), std::string("my-project")}, 
            object::pair{std::string("packageType"), std::string("project")}, 
            object::pair{std::string("dependencies"), object{
                object::pair{std::string("@elizaos/core"), std::string("^1.0.0")}
            }}
        }}, 
        object::pair{std::string("elizaPlugin"), object{
            object::pair{std::string("name"), std::string("@elizaos/plugin-test")}, 
            object::pair{std::string("packageType"), std::string("plugin")}, 
            object::pair{std::string("dependencies"), object{
                object::pair{std::string("@elizaos/core"), std::string("^1.0.0")}
            }}
        }}, 
        object::pair{std::string("elizaPluginByKeywords"), object{
            object::pair{std::string("name"), std::string("custom-plugin")}, 
            object::pair{std::string("keywords"), array<string>{ std::string("plugin"), std::string("elizaos") }}, 
            object::pair{std::string("dependencies"), object{
                object::pair{std::string("@elizaos/core"), std::string("^1.0.0")}
            }}
        }}, 
        object::pair{std::string("monorepoRoot"), object{
            object::pair{std::string("name"), std::string("monorepo-root")}, 
            object::pair{std::string("workspaces"), array<string>{ std::string("packages/*") }}
        }}, 
        object::pair{std::string("multipleElizaDeps"), object{
            object::pair{std::string("name"), std::string("my-project")}, 
            object::pair{std::string("dependencies"), object{
                object::pair{std::string("@elizaos/core"), std::string("^1.0.0")}, 
                object::pair{std::string("@elizaos/cli"), std::string("^1.0.0")}, 
                object::pair{std::string("@elizaos/plugin-discord"), std::string("^1.0.0")}
            }}
        }}, 
        object::pair{std::string("regularProject"), object{
            object::pair{std::string("name"), std::string("regular-project")}, 
            object::pair{std::string("dependencies"), object{
                object::pair{std::string("express"), std::string("^4.0.0")}, 
                object::pair{std::string("react"), std::string("^18.0.0")}
            }}
        }}
    }}, 
    object::pair{std::string("paths"), object{
        object::pair{std::string("testPath"), std::string("/test/path")}, 
        object::pair{std::string("testPlugin"), std::string("/test/plugin")}, 
        object::pair{std::string("testMonorepo"), std::string("/test/monorepo")}, 
        object::pair{std::string("testSubdir"), std::string("/test/monorepo/subdir")}, 
        object::pair{std::string("testRegular"), std::string("/test/regular")}, 
        object::pair{std::string("testMissing"), std::string("/test/missing")}, 
        object::pair{std::string("testInvalid"), std::string("/test/invalid")}, 
        object::pair{std::string("testUnreadable"), std::string("/test/unreadable")}
    }}, 
    object::pair{std::string("directoryInfo"), object{
        object::pair{std::string("elizaProject"), object{
            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("elizaos-project"))}, 
            object::pair{std::string("hasPackageJson"), true}, 
            object::pair{std::string("hasElizaOSDependencies"), true}, 
            object::pair{std::string("elizaPackageCount"), 1}
        }}, 
        object::pair{std::string("elizaPlugin"), object{
            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("elizaos-plugin"))}, 
            object::pair{std::string("hasPackageJson"), true}, 
            object::pair{std::string("hasElizaOSDependencies"), true}, 
            object::pair{std::string("elizaPackageCount"), 1}
        }}, 
        object::pair{std::string("elizaMonorepo"), object{
            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("elizaos-monorepo"))}, 
            object::pair{std::string("hasPackageJson"), true}, 
            object::pair{std::string("hasElizaOSDependencies"), false}, 
            object::pair{std::string("elizaPackageCount"), 0}
        }}, 
        object::pair{std::string("elizaSubdir"), object{
            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("elizaos-subdir"))}, 
            object::pair{std::string("hasPackageJson"), false}, 
            object::pair{std::string("hasElizaOSDependencies"), false}, 
            object::pair{std::string("elizaPackageCount"), 0}
        }}, 
        object::pair{std::string("nonElizaDir"), object{
            object::pair{std::string("type"), as<std::shared_ptr<const>>(std::string("non-elizaos-dir"))}, 
            object::pair{std::string("hasPackageJson"), true}, 
            object::pair{std::string("hasElizaOSDependencies"), false}, 
            object::pair{std::string("elizaPackageCount"), 0}
        }}
    }}
};
any mockFindMonorepoRoot = mock();
object mockUserEnvironmentInstance = object{
    object::pair{std::string("findMonorepoRoot"), mockFindMonorepoRoot}
};

void Main(void)
{
    mock->module(std::string("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std::string("existsSync"), mock([=]() mutable
            {
                return true;
            }
            )}, 
            object::pair{std::string("readFileSync"), mock([=]() mutable
            {
                return std::string("{}");
            }
            )}, 
            object::pair{std::string("statSync"), mock([=]() mutable
            {
                return (object{
                    object::pair{std::string("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
            }
            )}, 
            object::pair{std::string("readdirSync"), mock([=]() mutable
            {
                return array<any>();
            }
            )}
        });
    }
    );
    mock->module(std::string("../../../src/utils/user-environment"), [=]() mutable
    {
        return (object{
            object::pair{std::string("UserEnvironment"), object{
                object::pair{std::string("getInstance"), mock([=]() mutable
                {
                    return mockUserEnvironmentInstance;
                }
                )}
            }}
        });
    }
    );
    describe(std::string("directory-detection"), [=]() mutable
    {
        shared<object> mocks;
        beforeEach([=]() mutable
        {
            mockFindMonorepoRoot->mockClear();
            (as<any>(fs->existsSync))["mockClear"]();
            (as<any>(fs->readFileSync))["mockClear"]();
            (as<any>(fs->readdirSync))["mockClear"]();
            (as<any>(fs->statSync))["mockClear"]();
            mocks = object{
                object::pair{std::string("findMonorepoRoot"), mockFindMonorepoRoot}, 
                object::pair{std::string("existsSync"), as<any>(fs->existsSync)}, 
                object::pair{std::string("readFileSync"), as<any>(fs->readFileSync)}, 
                object::pair{std::string("readdirSync"), as<any>(fs->readdirSync)}, 
                object::pair{std::string("statSync"), as<any>(fs->statSync)}
            };
            mocks["existsSync"]["mockReturnValue"](true);
            mocks["readFileSync"]["mockReturnValue"](std::string("{}"));
            mocks["readdirSync"]["mockReturnValue"](array<any>());
            mocks["statSync"]["mockReturnValue"](object{
                object::pair{std::string("isDirectory"), [=]() mutable
                {
                    return true;
                }
                }
            });
            mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
        }
        );
        describe(std::string("detectDirectoryType"), [=]() mutable
        {
            it(std::string("should detect elizaos project"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["elizaProject"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPath"]);
                expect(result->type)->toBe(std::string("elizaos-project"));
                expect(result->hasPackageJson)->toBe(true);
                expect(result->hasElizaOSDependencies)->toBe(true);
                expect(result->elizaPackageCount)->toBe(1);
            }
            );
            it(std::string("should detect elizaos plugin"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["elizaPlugin"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPlugin"]);
                expect(result->type)->toBe(std::string("elizaos-plugin"));
                expect(result->hasPackageJson)->toBe(true);
            }
            );
            it(std::string("should detect monorepo root"), [=]() mutable
            {
                mocks["existsSync"]["mockImplementation"]([=](auto filepath) mutable
                {
                    auto pathStr = String(filepath);
                    return OR((pathStr->includes(std::string("package.json"))), (pathStr == TestFixtures["paths"]["testMonorepo"]));
                }
                );
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["monorepoRoot"]));
                mocks["readdirSync"]["mockReturnValue"](array<string>{ std::string("packages") });
                mocks["statSync"]["mockReturnValue"](object{
                    object::pair{std::string("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
                mocks["findMonorepoRoot"]["mockReturnValue"](TestFixtures["paths"]["testMonorepo"]);
                auto result = detectDirectoryType(TestFixtures["paths"]["testMonorepo"]);
                expect(result->type)->toBe(std::string("elizaos-monorepo"));
                expect(result->monorepoRoot)->toBe(TestFixtures["paths"]["testMonorepo"]);
            }
            );
            it(std::string("should detect elizaos subdirectory in monorepo"), [=]() mutable
            {
                mocks["existsSync"]["mockImplementation"]([=](auto filepath) mutable
                {
                    auto pathStr = String(filepath);
                    if (pathStr->includes(std::string("package.json"))) {
                        return false;
                    }
                    return pathStr == TestFixtures["paths"]["testSubdir"];
                }
                );
                mocks["readdirSync"]["mockReturnValue"](array<string>{ std::string("some-file.txt") });
                mocks["statSync"]["mockReturnValue"](object{
                    object::pair{std::string("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
                mocks["findMonorepoRoot"]["mockReturnValue"](TestFixtures["paths"]["testMonorepo"]);
                auto result = detectDirectoryType(TestFixtures["paths"]["testSubdir"]);
                expect(result->type)->toBe(std::string("elizaos-subdir"));
                expect(result->hasPackageJson)->toBe(false);
                expect(result->monorepoRoot)->toBe(TestFixtures["paths"]["testMonorepo"]);
            }
            );
            it(std::string("should return non-elizaos-dir for regular project"), [=]() mutable
            {
                mocks["existsSync"]["mockImplementation"]([=](auto filepath) mutable
                {
                    auto pathStr = String(filepath);
                    return pathStr->includes(std::string("package.json"));
                }
                );
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["regularProject"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["statSync"]["mockReturnValue"](object{
                    object::pair{std::string("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testRegular"]);
                expect(result->type)->toBe(std::string("non-elizaos-dir"));
                expect(result->hasElizaOSDependencies)->toBe(false);
            }
            );
            it(std::string("should handle missing directory"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](false);
                auto result = detectDirectoryType(TestFixtures["paths"]["testMissing"]);
                expect(result->type)->toBe(std::string("non-elizaos-dir"));
                expect(result->hasPackageJson)->toBe(false);
            }
            );
            it(std::string("should handle invalid JSON in package.json"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](std::string("invalid json"));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testInvalid"]);
                expect(result->type)->toBe(std::string("non-elizaos-dir"));
                expect(result->hasPackageJson)->toBe(true);
            }
            );
            it(std::string("should count multiple elizaos packages"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["multipleElizaDeps"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPath"]);
                expect(result->elizaPackageCount)->toBe(3);
                expect(result->hasElizaOSDependencies)->toBe(true);
            }
            );
            it(std::string("should detect plugin by keywords"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["elizaPluginByKeywords"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPlugin"]);
                expect(result->type)->toBe(std::string("elizaos-plugin"));
            }
            );
            it(std::string("should handle unreadable directory"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readdirSync"]["mockImplementation"]([=]() mutable
                {
                    throw any(std::make_shared<Error>(std::string("Permission denied")));
                }
                );
                auto result = detectDirectoryType(TestFixtures["paths"]["testUnreadable"]);
                expect(result->type)->toBe(std::string("non-elizaos-dir"));
                expect(result->hasPackageJson)->toBe(false);
            }
            );
        }
        );
        describe(std::string("isValidForUpdates"), [=]() mutable
        {
            it(std::string("should return true for elizaos-project"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaProject"]))->toBe(true);
            }
            );
            it(std::string("should return true for elizaos-plugin"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaPlugin"]))->toBe(true);
            }
            );
            it(std::string("should return true for elizaos-monorepo"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaMonorepo"]))->toBe(true);
            }
            );
            it(std::string("should return true for elizaos-subdir"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaSubdir"]))->toBe(true);
            }
            );
            it(std::string("should return false for non-elizaos-dir"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["nonElizaDir"]))->toBe(false);
            }
            );
        }
        );
    }
    );
}

MAIN
