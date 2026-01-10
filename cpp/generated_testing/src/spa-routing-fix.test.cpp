#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/agentserver/src/__tests__/spa-routing-fix.test.h"

void Main(void)
{
    describe(std::string("SPA Routing Fix"), [=]() mutable
    {
        shared<string> tempDir;
        beforeEach([=]() mutable
        {
            tempDir = path->join(os->tmpdir(), std::string("eliza-spa-test-") + Date->now() + string_empty);
            mkdirSync(tempDir, object{
                object::pair{std::string("recursive"), true}
            });
        }
        );
        afterEach([=]() mutable
        {
            if (existsSync(tempDir)) {
                rmSync(tempDir, object{
                    object::pair{std::string("recursive"), true}, 
                    object::pair{std::string("force"), true}
                });
            }
        }
        );
        describe(std::string("Client Path Resolution"), [=]() mutable
        {
            it(std::string("should find index.html when explicitly provided via options"), [=]() mutable
            {
                auto clientPath = path->join(tempDir, std::string("dist"));
                mkdirSync(clientPath, object{
                    object::pair{std::string("recursive"), true}
                });
                writeFileSync(path->join(clientPath, std::string("index.html")), std::string("<html></html>"));
                expect(existsSync(path->join(clientPath, std::string("index.html"))))->toBe(true);
            }
            );
            it(std::string("should handle missing index.html gracefully"), [=]() mutable
            {
                auto clientPath = path->join(tempDir, std::string("dist"));
                mkdirSync(clientPath, object{
                    object::pair{std::string("recursive"), true}
                });
                expect(existsSync(path->join(clientPath, std::string("index.html"))))->toBe(false);
            }
            );
            it(std::string("should resolve absolute paths correctly"), [=]() mutable
            {
                auto clientPath = path->join(tempDir, std::string("dist"));
                auto indexPath = path->join(clientPath, std::string("index.html"));
                expect(path->isAbsolute(indexPath))->toBe(true);
            }
            );
        }
        );
        describe(std::string("CLI Path Resolution"), [=]() mutable
        {
            it(std::string("should calculate correct dist path from nested directory"), [=]() mutable
            {
                auto cliRoot = path->join(tempDir, std::string("cli"));
                auto commandsDir = path->join(cliRoot, std::string("dist"), std::string("commands"), std::string("start"), std::string("actions"));
                mkdirSync(commandsDir, object{
                    object::pair{std::string("recursive"), true}
                });
                writeFileSync(path->join(cliRoot, std::string("dist"), std::string("index.html")), std::string("<html></html>"));
                auto resolvedPath = path->resolve(commandsDir, std::string("../../../"));
                expect(resolvedPath)->toBe(path->join(cliRoot, std::string("dist")));
                expect(existsSync(path->join(resolvedPath, std::string("index.html"))))->toBe(true);
            }
            );
            it(std::string("should find package.json and resolve dist directory"), [=]() mutable
            {
                auto pkgRoot = path->join(tempDir, std::string("node_modules"), std::string("@elizaos"), std::string("cli"));
                auto distDir = path->join(pkgRoot, std::string("dist"));
                mkdirSync(distDir, object{
                    object::pair{std::string("recursive"), true}
                });
                writeFileSync(path->join(pkgRoot, std::string("package.json")), JSON->stringify(object{
                    object::pair{std::string("name"), std::string("@elizaos/cli")}, 
                    object::pair{std::string("version"), std::string("1.0.0")}
                }));
                writeFileSync(path->join(distDir, std::string("index.html")), std::string("<html></html>"));
                expect(existsSync(path->join(pkgRoot, std::string("package.json"))))->toBe(true);
                expect(existsSync(path->join(distDir, std::string("index.html"))))->toBe(true);
            }
            );
        }
        );
    }
    );
}

MAIN
