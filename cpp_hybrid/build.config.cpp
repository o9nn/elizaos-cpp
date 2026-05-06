#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-personality/build.config.h"

std::shared_ptr<BuildConfig> buildConfig = object{
    object::pair{std::string("entrypoints"), array<string>{ std::string("./src/index.ts") }}, 
    object::pair{std::string("outdir"), std::string("./dist")}, 
    object::pair{std::string("target"), std::string("node")}, 
    object::pair{std::string("format"), std::string("esm")}, 
    object::pair{std::string("splitting"), false}, 
    object::pair{std::string("sourcemap"), std::string("external")}, 
    object::pair{std::string("external"), array<string>{ std::string("fs"), std::string("path"), std::string("http"), std::string("https"), std::string("crypto"), std::string("node:fs"), std::string("node:path"), std::string("node:http"), std::string("node:https"), std::string("node:crypto"), std::string("node:stream"), std::string("node:buffer"), std::string("node:util"), std::string("node:events"), std::string("node:url"), std::string("bun:test"), std::string("dotenv"), std::string("zod"), std::string("@elizaos/core"), std::string("fs-extra"), std::string("events"), std::string("pg") }}, 
    object::pair{std::string("naming"), std::string("[dir]/[name].[ext]")}
};

void Main(void)
{
}

MAIN
