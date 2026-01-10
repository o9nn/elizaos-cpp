#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/cli/src/utils/module-loader.test.h"

void Main(void)
{
    describe(std::string("ModuleLoader"), [=]() mutable
    {
        shared<std::shared_ptr<ModuleLoader>> loader;
        beforeEach([=]() mutable
        {
            (as<any>(global))["defaultLoader"] = nullptr;
            loader = std::make_shared<ModuleLoader>();
        }
        );
        describe(std::string("loadSync"), [=]() mutable
        {
            it(std::string("should load a module synchronously"), [=]() mutable
            {
                auto pathModule = loader->loadSync(std::string("path"));
                expect(pathModule)->toBeDefined();
                expect(pathModule["join"])->toBeDefined();
                expect(type_of(pathModule["join"]))->toBe(std::string("function"));
            }
            );
            it(std::string("should cache modules for repeated calls"), [=]() mutable
            {
                auto firstLoad = loader->loadSync(std::string("path"));
                auto secondLoad = loader->loadSync(std::string("path"));
                expect(firstLoad)->toBe(secondLoad);
            }
            );
            it(std::string("should maintain separate caches for sync and async loads"), [=]() mutable
            {
                auto syncLoad = loader->loadSync(std::string("path"));
                auto asyncLoad = std::async([=]() { loader->load(std::string("path")); });
                expect(syncLoad)->not->toBe(asyncLoad);
            }
            );
            it(std::string("should throw meaningful error for non-existent modules"), [=]() mutable
            {
                expect([=]() mutable
                {
                    loader->loadSync(std::string("non-existent-module-12345"));
                }
                )->toThrowError((new RegExp(std::string("Cannot find module 'non-existent-module-12345"))));
            }
            );
        }
        );
        describe(std::string("convenience functions"), [=]() mutable
        {
            it(std::string("loadModuleSync should work correctly"), [=]() mutable
            {
                auto pathModule = loadModuleSync(std::string("path"));
                expect(pathModule)->toBeDefined();
                expect(pathModule["join"])->toBeDefined();
            }
            );
            it(std::string("should maintain separate caches for sync and async loads"), [=]() mutable
            {
                auto syncLoad = loadModuleSync(std::string("path"));
                auto asyncLoad = std::async([=]() { loadModule(std::string("path")); });
                expect(syncLoad)->not->toBe(asyncLoad);
            }
            );
        }
        );
        describe(std::string("singleton behavior"), [=]() mutable
        {
            it(std::string("getModuleLoader should return the same instance"), [=]() mutable
            {
                auto loader1 = getModuleLoader();
                auto loader2 = getModuleLoader();
                expect(loader1)->toBe(loader2);
            }
            );
            it(std::string("modules loaded through singleton should be cached"), [=]() mutable
            {
                auto load1 = loadModuleSync(std::string("path"));
                auto load2 = loadModuleSync(std::string("path"));
                expect(load1)->toBe(load2);
            }
            );
        }
        );
    }
    );
}

MAIN
