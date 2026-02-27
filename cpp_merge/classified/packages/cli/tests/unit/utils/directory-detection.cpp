#include "directory-detection.test.h"

object TestFixtures = object{
    object::pair{std:("packageJson"), object{
        object::pair{std:("elizaProject"), object{
            object::pair{std:("name"), std:("my-project")}, 
            object::pair{std:("packageType"), std:("project")}, 
            object::pair{std:("dependencies"), object{
                object::pair{std:("@elizaos/core"), std:("^1.0.0")}
            }}
        }}, 
        object::pair{std:("elizaPlugin"), object{
            object::pair{std:("name"), std:("@elizaos/plugin-test")}, 
            object::pair{std:("packageType"), std:("plugin")}, 
            object::pair{std:("dependencies"), object{
                object::pair{std:("@elizaos/core"), std:("^1.0.0")}
            }}
        }}, 
        object::pair{std:("elizaPluginByKeywords"), object{
            object::pair{std:("name"), std:("custom-plugin")}, 
            object::pair{std:("keywords"), array<string>{ std:("plugin"), std:("elizaos") }}, 
            object::pair{std:("dependencies"), object{
                object::pair{std:("@elizaos/core"), std:("^1.0.0")}
            }}
        }}, 
        object::pair{std:("monorepoRoot"), object{
            object::pair{std:("name"), std:("monorepo-root")}, 
            object::pair{std:("workspaces"), array<string>{ std:("packages/*") }}
        }}, 
        object::pair{std:("multipleElizaDeps"), object{
            object::pair{std:("name"), std:("my-project")}, 
            object::pair{std:("dependencies"), object{
                object::pair{std:("@elizaos/core"), std:("^1.0.0")}, 
                object::pair{std:("@elizaos/cli"), std:("^1.0.0")}, 
                object::pair{std:("@elizaos/plugin-discord"), std:("^1.0.0")}
            }}
        }}, 
        object::pair{std:("regularProject"), object{
            object::pair{std:("name"), std:("regular-project")}, 
            object::pair{std:("dependencies"), object{
                object::pair{std:("express"), std:("^4.0.0")}, 
                object::pair{std:("react"), std:("^18.0.0")}
            }}
        }}
    }}, 
    object::pair{std:("paths"), object{
        object::pair{std:("testPath"), std:("/test/path")}, 
        object::pair{std:("testPlugin"), std:("/test/plugin")}, 
        object::pair{std:("testMonorepo"), std:("/test/monorepo")}, 
        object::pair{std:("testSubdir"), std:("/test/monorepo/subdir")}, 
        object::pair{std:("testRegular"), std:("/test/regular")}, 
        object::pair{std:("testMissing"), std:("/test/missing")}, 
        object::pair{std:("testInvalid"), std:("/test/invalid")}, 
        object::pair{std:("testUnreadable"), std:("/test/unreadable")}
    }}, 
    object::pair{std:("directoryInfo"), object{
        object::pair{std:("elizaProject"), object{
            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("elizaos-project"))}, 
            object::pair{std:("hasPackageJson"), true}, 
            object::pair{std:("hasElizaOSDependencies"), true}, 
            object::pair{std:("elizaPackageCount"), 1}
        }}, 
        object::pair{std:("elizaPlugin"), object{
            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("elizaos-plugin"))}, 
            object::pair{std:("hasPackageJson"), true}, 
            object::pair{std:("hasElizaOSDependencies"), true}, 
            object::pair{std:("elizaPackageCount"), 1}
        }}, 
        object::pair{std:("elizaMonorepo"), object{
            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("elizaos-monorepo"))}, 
            object::pair{std:("hasPackageJson"), true}, 
            object::pair{std:("hasElizaOSDependencies"), false}, 
            object::pair{std:("elizaPackageCount"), 0}
        }}, 
        object::pair{std:("elizaSubdir"), object{
            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("elizaos-subdir"))}, 
            object::pair{std:("hasPackageJson"), false}, 
            object::pair{std:("hasElizaOSDependencies"), false}, 
            object::pair{std:("elizaPackageCount"), 0}
        }}, 
        object::pair{std:("nonElizaDir"), object{
            object::pair{std:("type"), as<std::shared_ptr<const>>(std:("non-elizaos-dir"))}, 
            object::pair{std:("hasPackageJson"), true}, 
            object::pair{std:("hasElizaOSDependencies"), false}, 
            object::pair{std:("elizaPackageCount"), 0}
        }}
    }}
};
any mockFindMonorepoRoot = mock();
object mockUserEnvironmentInstance = object{
    object::pair{std:("findMonorepoRoot"), mockFindMonorepoRoot}
};

void Main(void)
{
    mock->module(std:("node:fs"), [=]() mutable
    {
        return (object{
            object::pair{std:("existsSync"), mock([=]() mutable
            {
                return true;
            }
            )}, 
            object::pair{std:("readFileSync"), mock([=]() mutable
            {
                return std:("{}");
            }
            )}, 
            object::pair{std:("statSync"), mock([=]() mutable
            {
                return (object{
                    object::pair{std:("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
            }
            )}, 
            object::pair{std:("readdirSync"), mock([=]() mutable
            {
                return array<any>();
            }
            )}
        });
    }
    );
    mock->module(std:("../../../src/utils/user-environment"), [=]() mutable
    {
        return (object{
            object::pair{std:("UserEnvironment"), object{
                object::pair{std:("getInstance"), mock([=]() mutable
                {
                    return mockUserEnvironmentInstance;
                }
                )}
            }}
        });
    }
    );
    describe(std:("directory-detection"), [=]() mutable
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
                object::pair{std:("findMonorepoRoot"), mockFindMonorepoRoot}, 
                object::pair{std:("existsSync"), as<any>(fs->existsSync)}, 
                object::pair{std:("readFileSync"), as<any>(fs->readFileSync)}, 
                object::pair{std:("readdirSync"), as<any>(fs->readdirSync)}, 
                object::pair{std:("statSync"), as<any>(fs->statSync)}
            };
            mocks["existsSync"]["mockReturnValue"](true);
            mocks["readFileSync"]["mockReturnValue"](std:("{}"));
            mocks["readdirSync"]["mockReturnValue"](array<any>());
            mocks["statSync"]["mockReturnValue"](object{
                object::pair{std:("isDirectory"), [=]() mutable
                {
                    return true;
                }
                }
            });
            mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
        }
        );
        describe(std:("detectDirectoryType"), [=]() mutable
        {
            it(std:("should detect elizaos project"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["elizaProject"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPath"]);
                expect(result->type)->toBe(std:("elizaos-project"));
                expect(result->hasPackageJson)->toBe(true);
                expect(result->hasElizaOSDependencies)->toBe(true);
                expect(result->elizaPackageCount)->toBe(1);
            }
            );
            it(std:("should detect elizaos plugin"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["elizaPlugin"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPlugin"]);
                expect(result->type)->toBe(std:("elizaos-plugin"));
                expect(result->hasPackageJson)->toBe(true);
            }
            );
            it(std:("should detect monorepo root"), [=]() mutable
            {
                mocks["existsSync"]["mockImplementation"]([=](auto filepath) mutable
                {
                    auto pathStr = String(filepath);
                    return OR((pathStr->includes(std:("package.json"))), (pathStr == TestFixtures["paths"]["testMonorepo"]));
                }
                );
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["monorepoRoot"]));
                mocks["readdirSync"]["mockReturnValue"](array<string>{ std:("packages") });
                mocks["statSync"]["mockReturnValue"](object{
                    object::pair{std:("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
                mocks["findMonorepoRoot"]["mockReturnValue"](TestFixtures["paths"]["testMonorepo"]);
                auto result = detectDirectoryType(TestFixtures["paths"]["testMonorepo"]);
                expect(result->type)->toBe(std:("elizaos-monorepo"));
                expect(result->monorepoRoot)->toBe(TestFixtures["paths"]["testMonorepo"]);
            }
            );
            it(std:("should detect elizaos subdirectory in monorepo"), [=]() mutable
            {
                mocks["existsSync"]["mockImplementation"]([=](auto filepath) mutable
                {
                    auto pathStr = String(filepath);
                    if (pathStr->includes(std:("package.json"))) {
                        return false;
                    }
                    return pathStr == TestFixtures["paths"]["testSubdir"];
                }
                );
                mocks["readdirSync"]["mockReturnValue"](array<string>{ std:("some-file.txt") });
                mocks["statSync"]["mockReturnValue"](object{
                    object::pair{std:("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
                mocks["findMonorepoRoot"]["mockReturnValue"](TestFixtures["paths"]["testMonorepo"]);
                auto result = detectDirectoryType(TestFixtures["paths"]["testSubdir"]);
                expect(result->type)->toBe(std:("elizaos-subdir"));
                expect(result->hasPackageJson)->toBe(false);
                expect(result->monorepoRoot)->toBe(TestFixtures["paths"]["testMonorepo"]);
            }
            );
            it(std:("should return non-elizaos-dir for regular project"), [=]() mutable
            {
                mocks["existsSync"]["mockImplementation"]([=](auto filepath) mutable
                {
                    auto pathStr = String(filepath);
                    return pathStr->includes(std:("package.json"));
                }
                );
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["regularProject"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["statSync"]["mockReturnValue"](object{
                    object::pair{std:("isDirectory"), [=]() mutable
                    {
                        return true;
                    }
                    }
                });
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testRegular"]);
                expect(result->type)->toBe(std:("non-elizaos-dir"));
                expect(result->hasElizaOSDependencies)->toBe(false);
            }
            );
            it(std:("should handle missing directory"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](false);
                auto result = detectDirectoryType(TestFixtures["paths"]["testMissing"]);
                expect(result->type)->toBe(std:("non-elizaos-dir"));
                expect(result->hasPackageJson)->toBe(false);
            }
            );
            it(std:("should handle invalid JSON in package.json"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](std:("invalid json"));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testInvalid"]);
                expect(result->type)->toBe(std:("non-elizaos-dir"));
                expect(result->hasPackageJson)->toBe(true);
            }
            );
            it(std:("should count multiple elizaos packages"), [=]() mutable
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
            it(std:("should detect plugin by keywords"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readFileSync"]["mockReturnValue"](JSON->stringify(TestFixtures["packageJson"]["elizaPluginByKeywords"]));
                mocks["readdirSync"]["mockReturnValue"](array<any>());
                mocks["findMonorepoRoot"]["mockReturnValue"](nullptr);
                auto result = detectDirectoryType(TestFixtures["paths"]["testPlugin"]);
                expect(result->type)->toBe(std:("elizaos-plugin"));
            }
            );
            it(std:("should handle unreadable directory"), [=]() mutable
            {
                mocks["existsSync"]["mockReturnValue"](true);
                mocks["readdirSync"]["mockImplementation"]([=]() mutable
                {
                    throw any(std::make_shared<Error>(std:("Permission denied")));
                }
                );
                auto result = detectDirectoryType(TestFixtures["paths"]["testUnreadable"]);
                expect(result->type)->toBe(std:("non-elizaos-dir"));
                expect(result->hasPackageJson)->toBe(false);
            }
            );
        }
        );
        describe(std:("isValidForUpdates"), [=]() mutable
        {
            it(std:("should return true for elizaos-project"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaProject"]))->toBe(true);
            }
            );
            it(std:("should return true for elizaos-plugin"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaPlugin"]))->toBe(true);
            }
            );
            it(std:("should return true for elizaos-monorepo"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaMonorepo"]))->toBe(true);
            }
            );
            it(std:("should return true for elizaos-subdir"), [=]() mutable
            {
                expect(isValidForUpdates(TestFixtures["directoryInfo"]["elizaSubdir"]))->toBe(true);
            }
            );
            it(std:("should return false for non-elizaos-dir"), [=]() mutable
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
