#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/server/build.h"

std::function<std::shared_ptr<Promise<void>>()> run = createBuildRunner(object{
    object::pair{std:("packageName"), std:("@elizaos/server")}, 
    object::pair{std:("buildOptions"), object{
        object::pair{std:("entrypoints"), array<string>{ std:("src/index.ts") }}, 
        object::pair{std:("outdir"), std:("dist")}, 
        object::pair{std:("target"), std:("node")}, 
        object::pair{std:("format"), std:("esm")}, 
        object::pair{std:("external"), array<string>{ std:("@elizaos/core"), std:("@elizaos/client"), std:("express"), std:("cors"), std:("multer"), std:("swagger-ui-express"), std:("@elizaos/plugin-sql"), std:("lancedb"), std:("vectordb"), std:("socket.io"), std:("discord.js"), std:("@/managers/*"), std:("@/constants/*") }}, 
        object::pair{std:("sourcemap"), false}, 
        object::pair{std:("minify"), false}, 
        object::pair{std:("generateDts"), false}
    }}, 
    object::pair{std:("onBuildComplete"), [=](auto success) mutable
    {
        if (success) {
            auto copyTasks = array<std::shared_ptr<Promise<void>>>();
            auto clientDistPath = join(process->cwd(), std:("../client/dist"));
            auto resolvedClientDist = nullptr;
            if (existsSync(clientDistPath)) {
                resolvedClientDist = clientDistPath;
            } else {
                try
                {
                    auto clientPkgPath = require->resolve(std:("@elizaos/client/package.json"), object{
                        object::pair{std:("paths"), array<string>{ process->cwd() }}
                    });
                    auto clientPkgDir = clientPkgPath->substring(0, clientPkgPath->lastIndexOf(std:("/")));
                    auto installedClientDist = join(clientPkgDir, std:("dist"));
                    if (existsSync(installedClientDist)) {
                        resolvedClientDist = installedClientDist;
                    }
                }
                catch (const any& _)
                {
                }
            }
            if (resolvedClientDist) {
                console->log(std:("\
Copying client assets..."));
                copyTasks->push(copyAssets(array<object>{ object{
                    object::pair{std:("from"), resolvedClientDist}, 
                    object::pair{std:("to"), std:("./dist/client")}
                } })->then([=]() mutable
                {
                    return console->log(std:(" Client assets copied"));
                }
                ));
            } else {
                console->warn(std:("  Client assets not found. The web UI will not be bundled into @elizaos/server."));
            }
            if (existsSync(std:("./public"))) {
                console->log(std:("Copying static assets..."));
                copyTasks->push(copyAssets(array<object>{ object{
                    object::pair{std:("from"), std:("./public")}, 
                    object::pair{std:("to"), std:("./dist/public")}
                } })->then([=]() mutable
                {
                    return console->log(std:(" Static assets copied"));
                }
                ));
            }
            if (copyTasks->get_length() > 0) {
                auto copyStart = performance->now();
                std::async([=]() { Promise->all(copyTasks); });
                auto copyDuration = ((performance->now() - copyStart) / 1000)->toFixed(2);
                console->log(std:(" All assets copied in parallel (") + copyDuration + std:("s)"));
            }
        }
    }
    }
});

void Main(void)
{
    run()->_catch([=](auto error) mutable
    {
        console->error(std:("Build script error:"), error);
        process->exit(1);
    }
    );
}

MAIN
