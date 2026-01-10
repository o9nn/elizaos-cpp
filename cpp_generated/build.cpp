#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/build.h"

std::function<std::shared_ptr<Promise<void>>()> run = createBuildRunner(object{
    object::pair{std::string("packageName"), std::string("@elizaos/api-client")}, 
    object::pair{std::string("buildOptions"), object{
        object::pair{std::string("entrypoints"), array<string>{ std::string("src/index.ts") }}, 
        object::pair{std::string("outdir"), std::string("dist")}, 
        object::pair{std::string("target"), std::string("node")}, 
        object::pair{std::string("format"), std::string("esm")}, 
        object::pair{std::string("external"), array<string>{ std::string("@elizaos/core"), std::string("fs"), std::string("path") }}, 
        object::pair{std::string("sourcemap"), true}, 
        object::pair{std::string("minify"), false}, 
        object::pair{std::string("generateDts"), true}
    }}, 
    object::pair{std::string("onBuildComplete"), [=](auto success) mutable
    {
        if (success) {
            auto rootDtsContent = std::string("// Main client\
export { ElizaClient } from './client';\
\
// Base types\
export * from './types/base';\
\
// Domain types\
export * from './types/agents';\
export * from './types/messaging';\
export * from './types/memory';\
export * from './types/audio';\
export * from './types/media';\
export * from './types/server';\
export * from './types/system';\
export * from './types/sessions';\
export * from './types/runs';\
\
// Services (for advanced usage)\
export { AgentsService } from './services/agents';\
export { MessagingService } from './services/messaging';\
export { MemoryService } from './services/memory';\
export { AudioService } from './services/audio';\
export { MediaService } from './services/media';\
export { ServerService } from './services/server';\
export { SystemService } from './services/system';\
export { SessionsService } from './services/sessions';\
export { RunsService } from './services/runs';\
\
// Base client and error\
export { BaseApiClient, ApiError } from './lib/base-client';\
");
            std::async([=]() { Bun->write(std::string("./dist/index.d.ts"), rootDtsContent); });
            console->log(std::string(" Created root index.d.ts"));
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
