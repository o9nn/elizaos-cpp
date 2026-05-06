#include "build.hpp"
#include <string>

any result = std::async([=]() { build(object{
    object::pair{std::string("entrypoints"), array<string>{ std::string("src/index.ts") }}, 
    object::pair{std::string("outdir"), std::string("dist")}, 
    object::pair{std::string("target"), std::string("node")}, 
    object::pair{std::string("format"), std::string("esm")}, 
    object::pair{std::string("external"), array<string>{ std::string("@elizaos/core"), std::string("@elizaos/plugin-sql"), std::string("drizzle-orm"), std::string("uuid") }}, 
    object::pair{std::string("sourcemap"), true}, 
    object::pair{std::string("minify"), false}
}); });

void Main(void)
{
    if (result->success) {
        console->log(std::string("✓ Build successful"));
    } else {
        console->error(std::string("✗ Build failed"));
        process->exit(1);
    }
}

MAIN
