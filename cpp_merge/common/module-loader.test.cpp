#include "module-loader.test.h"

void Main(void)
{
    describe(std:("ModuleLoader"), [=]() mutable
    {
        shared<std::shared_ptr<ModuleLoader>> loader;
        beforeEach([=]() mutable
        {
            (as<any>(global))["defaultLoader"] = nullptr;
            loader = std::make_shared<ModuleLoader>();
        }
        );
        describe(std:("loadSync"), [=]() mutable
        {
            it(std:("should load a module synchronously"), [=]() mutable
            {
                auto pathModule = loader->loadSync(std:("path"));
                expect(pathModule)->toBeDefined();
                expect(pathModule["join"])->toBeDefined();
                expect(type_of(pathModule["join"]))->toBe(std:("function"));
            }
            );
            it(std:("should cache modules for repeated calls"), [=]() mutable
            {
                auto firstLoad = loader->loadSync(std:("path"));
                auto secondLoad = loader->loadSync(std:("path"));
                expect(firstLoad)->toBe(secondLoad);
            }
            );
            it(std:("should maintain separate caches for sync and async loads"), [=]() mutable
            {
                auto syncLoad = loader->loadSync(std:("path"));
                auto asyncLoad = std::async([=]() { loader->load(std:("path")); });
                expect(syncLoad)->not->toBe(asyncLoad);
            }
            );
            it(std:("should throw meaningful error for non-existent modules"), [=]() mutable
            {
                expect([=]() mutable
                {
                    loader->loadSync(std:("non-existent-module-12345"));
                }
                )->toThrowError((new RegExp(std:("Cannot find module 'non-existent-module-12345"))));
            }
            );
        }
        );
        describe(std:("convenience functions"), [=]() mutable
        {
            it(std:("loadModuleSync should work correctly"), [=]() mutable
            {
                auto pathModule = loadModuleSync(std:("path"));
                expect(pathModule)->toBeDefined();
                expect(pathModule["join"])->toBeDefined();
            }
            );
            it(std:("should maintain separate caches for sync and async loads"), [=]() mutable
            {
                auto syncLoad = loadModuleSync(std:("path"));
                auto asyncLoad = std::async([=]() { loadModule(std:("path")); });
                expect(syncLoad)->not->toBe(asyncLoad);
            }
            );
        }
        );
        describe(std:("singleton behavior"), [=]() mutable
        {
            it(std:("getModuleLoader should return the same instance"), [=]() mutable
            {
                auto loader1 = getModuleLoader();
                auto loader2 = getModuleLoader();
                expect(loader1)->toBe(loader2);
            }
            );
            it(std:("modules loaded through singleton should be cached"), [=]() mutable
            {
                auto load1 = loadModuleSync(std:("path"));
                auto load2 = loadModuleSync(std:("path"));
                expect(load1)->toBe(load2);
            }
            );
        }
        );
    }
    );
}

MAIN
