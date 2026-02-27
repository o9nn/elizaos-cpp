#include "build.hpp"

std::function<std::shared_ptr<Promise<void>>()> run = createBuildRunner(object{
    object::pair{std:("packageName"), std:("@elizaos/api-client")}, 
    object::pair{std:("buildOptions"), object{
        object::pair{std:("entrypoints"), array<string>{ std:("src/index.ts") }}, 
        object::pair{std:("outdir"), std:("dist")}, 
        object::pair{std:("target"), std:("node")}, 
        object::pair{std:("format"), std:("esm")}, 
        object::pair{std:("external"), array<string>{ std:("@elizaos/core"), std:("fs"), std:("path") }}, 
        object::pair{std:("sourcemap"), true}, 
        object::pair{std:("minify"), false}, 
        object::pair{std:("generateDts"), true}
    }}, 
    object::pair{std:("onBuildComplete"), [=](auto success) mutable
    {
        if (success) {
            auto rootDtsContent = std:("// Main client\
{ ElizaClient } from './client';\
\
// Base types\
* from './types/base';\
\
// Domain types\
* from './types/agents';\
* from './types/messaging';\
* from './types/memory';\
* from './types/audio';\
* from './types/media';\
* from './types/server';\
* from './types/system';\
* from './types/sessions';\
* from './types/runs';\
\
// Services (for advanced usage)\
{ AgentsService } from './services/agents';\
{ MessagingService } from './services/messaging';\
{ MemoryService } from './services/memory';\
{ AudioService } from './services/audio';\
{ MediaService } from './services/media';\
{ ServerService } from './services/server';\
{ SystemService } from './services/system';\
{ SessionsService } from './services/sessions';\
{ RunsService } from './services/runs';\
\
// Base client and error\
{ BaseApiClient, ApiError } from './lib/base-client';\
");
            std::async([=]() { Bun->write(std:("./dist/index.d.ts"), rootDtsContent); });
            console->log(std:(" Created root index.d.ts"));
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
