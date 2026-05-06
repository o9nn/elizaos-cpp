#include "build.hpp"
#include <string>

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
