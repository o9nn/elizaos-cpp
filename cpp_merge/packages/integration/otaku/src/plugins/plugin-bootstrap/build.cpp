#include "build.hpp"

std::function<std::shared_ptr<Promise<void>>()> run = createBuildRunner(object{
    object::pair{std:("packageName"), std:("@elizaos/plugin-bootstrap")}, 
    object::pair{std:("buildOptions"), object{
        object::pair{std:("entrypoints"), array<string>{ std:("src/index.ts") }}, 
        object::pair{std:("outdir"), std:("dist")}, 
        object::pair{std:("target"), std:("node")}, 
        object::pair{std:("format"), std:("esm")}, 
        object::pair{std:("external"), array<string>{ std:("dotenv"), std:("fs"), std:("path"), std:("@reflink/reflink"), std:("agentkeepalive"), std:("zod"), std:("@elizaos/core"), std:("@elizaos/plugin-sql") }}, 
        object::pair{std:("sourcemap"), true}, 
        object::pair{std:("minify"), false}, 
        object::pair{std:("generateDts"), true}
    }}
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
