#include "build.config.h"

std::shared_ptr<BuildConfig> buildConfig = object{
    object::pair{std:("entrypoints"), array<string>{ std:("./src/index.ts") }}, 
    object::pair{std:("outdir"), std:("./dist")}, 
    object::pair{std:("target"), std:("node")}, 
    object::pair{std:("format"), std:("esm")}, 
    object::pair{std:("splitting"), false}, 
    object::pair{std:("sourcemap"), std:("external")}, 
    object::pair{std:("external"), array<string>{ std:("fs"), std:("path"), std:("http"), std:("https"), std:("crypto"), std:("node:fs"), std:("node:path"), std:("node:http"), std:("node:https"), std:("node:crypto"), std:("node:stream"), std:("node:buffer"), std:("node:util"), std:("node:events"), std:("node:url"), std:("bun:test"), std:("dotenv"), std:("zod"), std:("typescript"), std:("execa"), std:("fs-extra"), std:("child_process"), std:("stream"), std:("util"), std:("os"), std:("url"), std:("@elizaos/core"), std:("punycode"), std:("node-fetch"), std:("agentkeepalive"), std:("chromium-bidi"), std:("cpu-features") }}, 
    object::pair{std:("naming"), std:("[dir]/[name].[ext]")}
};

void Main(void)
{
}

MAIN
