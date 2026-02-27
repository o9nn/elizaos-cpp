#include "spa-routing-fix.test.h"

void Main(void)
{
    describe(std:("SPA Routing Fix"), [=]() mutable
    {
        shared<string> tempDir;
        beforeEach([=]() mutable
        {
            tempDir = path->join(os->tmpdir(), std:("eliza-spa-test-") + Date->now() + string_empty);
            mkdirSync(tempDir, object{
                object::pair{std:("recursive"), true}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (existsSync(tempDir)) {
                rmSync(tempDir, object{
                    object::pair{std:("recursive"), true}, 
                    object::pair{std:("force"), true}
                });
            }
        }
        );
        describe(std:("Client Path Resolution"), [=]() mutable
        {
            it(std:("should find index.html when explicitly provided via options"), [=]() mutable
            {
                auto clientPath = path->join(tempDir, std:("dist"));
                mkdirSync(clientPath, object{
                    object::pair{std:("recursive"), true}
                });
                writeFileSync(path->join(clientPath, std:("index.html")), std:("<html></html>"));
                expect(existsSync(path->join(clientPath, std:("index.html"))))->toBe(true);
            }
            );
            it(std:("should handle missing index.html gracefully"), [=]() mutable
            {
                auto clientPath = path->join(tempDir, std:("dist"));
                mkdirSync(clientPath, object{
                    object::pair{std:("recursive"), true}
                });
                expect(existsSync(path->join(clientPath, std:("index.html"))))->toBe(false);
            }
            );
            it(std:("should resolve absolute paths correctly"), [=]() mutable
            {
                auto clientPath = path->join(tempDir, std:("dist"));
                auto indexPath = path->join(clientPath, std:("index.html"));
                expect(path->isAbsolute(indexPath))->toBe(true);
            }
            );
        }
        );
        describe(std:("CLI Path Resolution"), [=]() mutable
        {
            it(std:("should calculate correct dist path from nested directory"), [=]() mutable
            {
                auto cliRoot = path->join(tempDir, std:("cli"));
                auto commandsDir = path->join(cliRoot, std:("dist"), std:("commands"), std:("start"), std:("actions"));
                mkdirSync(commandsDir, object{
                    object::pair{std:("recursive"), true}
                });
                writeFileSync(path->join(cliRoot, std:("dist"), std:("index.html")), std:("<html></html>"));
                auto resolvedPath = path->resolve(commandsDir, std:("../../../"));
                expect(resolvedPath)->toBe(path->join(cliRoot, std:("dist")));
                expect(existsSync(path->join(resolvedPath, std:("index.html"))))->toBe(true);
            }
            );
            it(std:("should find package.json and resolve dist directory"), [=]() mutable
            {
                auto pkgRoot = path->join(tempDir, std:("node_modules"), std:("@elizaos"), std:("cli"));
                auto distDir = path->join(pkgRoot, std:("dist"));
                mkdirSync(distDir, object{
                    object::pair{std:("recursive"), true}
                });
                writeFileSync(path->join(pkgRoot, std:("package.json")), JSON->stringify(object{
                    object::pair{std:("name"), std:("@elizaos/cli")}, 
                    object::pair{std:("version"), std:("1.0.0")}
                }));
                writeFileSync(path->join(distDir, std:("index.html")), std:("<html></html>"));
                expect(existsSync(path->join(pkgRoot, std:("package.json"))))->toBe(true);
                expect(existsSync(path->join(distDir, std:("index.html"))))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
