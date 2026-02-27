#include "build.hpp"

any result = std::async([=]() { build(object{
    object::pair{std:("entrypoints"), array<string>{ std:("src/index.ts") }}, 
    object::pair{std:("outdir"), std:("dist")}, 
    object::pair{std:("target"), std:("node")}, 
    object::pair{std:("format"), std:("esm")}, 
    object::pair{std:("external"), array<string>{ std:("@elizaos/core"), std:("@elizaos/plugin-sql"), std:("drizzle-orm"), std:("uuid") }}, 
    object::pair{std:("sourcemap"), true}, 
    object::pair{std:("minify"), false}
}); });

void Main(void)
{
    if (result->success) {
        console->log(std:("✓ Build successful"));
    } else {
        console->error(std:("✗ Build failed"));
        process->exit(1);
    }
}

MAIN
