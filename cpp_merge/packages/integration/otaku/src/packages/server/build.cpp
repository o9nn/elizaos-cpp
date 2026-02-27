#include "build.hpp"
#include <string>

std::function<std::shared_ptr<Promise<void>>()> run = createBuildRunner(object{
    object::pair{std::string("packageName"), std::string("@elizaos/server")}, 
    object::pair{std::string("buildOptions"), object{
        object::pair{std::string("entrypoints"), array<string>{ std::string("src/index.ts") }}, 
        object::pair{std::string("outdir"), std::string("dist")}, 
        object::pair{std::string("target"), std::string("node")}, 
        object::pair{std::string("format"), std::string("esm")}, 
        object::pair{std::string("external"), array<string>{ std::string("@elizaos/core"), std::string("@elizaos/client"), std::string("express"), std::string("cors"), std::string("multer"), std::string("swagger-ui-express"), std::string("@elizaos/plugin-sql"), std::string("lancedb"), std::string("vectordb"), std::string("socket.io"), std::string("discord.js"), std::string("@/managers/*"), std::string("@/constants/*") }}, 
        object::pair{std::string("sourcemap"), false}, 
        object::pair{std::string("minify"), false}, 
        object::pair{std::string("generateDts"), false}
    }}, 
    object::pair{std::string("onBuildComplete"), [=](auto success) mutable
    {
        if (success) {
            auto copyTasks = array<std::shared_ptr<Promise<void>>>();
            auto clientDistPath = join(process->cwd(), std::string("../client/dist"));
            auto resolvedClientDist = nullptr;
            if (existsSync(clientDistPath)) {
                resolvedClientDist = clientDistPath;
            } else {
                try
                {
                    auto clientPkgPath = require->resolve(std::string("@elizaos/client/package.json"), object{
                        object::pair{std::string("paths"), array<string>{ process->cwd() }}
                    });
                    auto clientPkgDir = clientPkgPath->substring(0, clientPkgPath->lastIndexOf(std::string("/")));
                    auto installedClientDist = join(clientPkgDir, std::string("dist"));
                    if (existsSync(installedClientDist)) {
                        resolvedClientDist = installedClientDist;
                    }
                }
                catch (const any& _)
                {
                }
            }
            if (resolvedClientDist) {
                console->log(std::string("\
Copying client assets..."));
                copyTasks->push(copyAssets(array<object>{ object{
                    object::pair{std::string("from"), resolvedClientDist}, 
                    object::pair{std::string("to"), std::string("./dist/client")}
                } })->then([=]() mutable
                {
                    return console->log(std::string(" Client assets copied"));
                }
                ));
            } else {
                console->warn(std::string("  Client assets not found. The web UI will not be bundled into @elizaos/server."));
            }
            if (existsSync(std::string("./public"))) {
                console->log(std::string("Copying static assets..."));
                copyTasks->push(copyAssets(array<object>{ object{
                    object::pair{std::string("from"), std::string("./public")}, 
                    object::pair{std::string("to"), std::string("./dist/public")}
                } })->then([=]() mutable
                {
                    return console->log(std::string(" Static assets copied"));
                }
                ));
            }
            if (copyTasks->get_length() > 0) {
                auto copyStart = performance->now();
                std::async([=]() { Promise->all(copyTasks); });
                auto copyDuration = ((performance->now() - copyStart) / 1000)->toFixed(2);
                console->log(std::string(" All assets copied in parallel (") + copyDuration + std::string("s)"));
            }
        }
    }
    }
});

void Main(void)
{
    run()->_catch([=](auto error) mutable
    {
        console->error(std::string("Build script error:"), error);
        process->exit(1);
    }
    );
}

MAIN
