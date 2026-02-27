#include "build.hpp"
#include <string>

std::function<std::shared_ptr<Promise<void>>()> run = createBuildRunner(object{
    object::pair{std::string("packageName"), std::string("@elizaos/plugin-bootstrap")}, 
    object::pair{std::string("buildOptions"), object{
        object::pair{std::string("entrypoints"), array<string>{ std::string("src/index.ts") }}, 
        object::pair{std::string("outdir"), std::string("dist")}, 
        object::pair{std::string("target"), std::string("node")}, 
        object::pair{std::string("format"), std::string("esm")}, 
        object::pair{std::string("external"), array<string>{ std::string("dotenv"), std::string("fs"), std::string("path"), std::string("@reflink/reflink"), std::string("agentkeepalive"), std::string("zod"), std::string("@elizaos/core"), std::string("@elizaos/plugin-sql") }}, 
        object::pair{std::string("sourcemap"), true}, 
        object::pair{std::string("minify"), false}, 
        object::pair{std::string("generateDts"), true}
    }}
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
