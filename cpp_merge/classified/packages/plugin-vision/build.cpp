#include "build.config.h"
#include <string>
#include <cstdlib>

std::shared_ptr<BuildConfig> buildConfig = object{
    object::pair{std::string("entrypoints"), array<string>{ std::string("./src/index.ts") }}, 
    object::pair{std::string("outdir"), std::string("./dist")}, 
    object::pair{std::string("target"), std::string("node")}, 
    object::pair{std::string("format"), std::string("esm")}, 
    object::pair{std::string("splitting"), false}, 
    object::pair{std::string("sourcemap"), std::string("external")}, 
    object::pair{std::string("external"), array<string>{ std::string("fs"), std::string("path"), std::string("http"), std::string("https"), std::string("crypto"), std::string("child_process"), std::string("os"), std::string("util"), std::string("stream"), std::string("buffer"), std::string("events"), std::string("url"), std::string("node:fs"), std::string("node:path"), std::string("node:http"), std::string("node:https"), std::string("node:crypto"), std::string("node:child_process"), std::string("node:os"), std::string("node:util"), std::string("node:stream"), std::string("node:buffer"), std::string("node:events"), std::string("node:url"), std::string("bun:test"), std::string("dotenv"), std::string("zod"), std::string("@elizaos/core"), std::string("@elizaos/plugin-bootstrap"), std::string("axios"), std::string("sharp"), std::string("@napi-rs/canvas"), std::string("face-api.js") }}, 
    object::pair{std::string("define"), object{
        object::pair{std::string("std::getenv("NODE_ENV")"), std::string(""production"")}
    }}, 
    object::pair{std::string("naming"), std::string("[dir]/[name].[ext]")}
};
std::shared_ptr<BuildConfig> workersConfig = object{
    object::pair{std::string("entrypoints"), array<string>{ std::string("./src/workers/screen-capture-worker.ts"), std::string("./src/workers/florence2-worker.ts"), std::string("./src/workers/ocr-worker.ts") }}, 
    object::pair{std::string("outdir"), std::string("./dist/workers")}, 
    object::pair{std::string("target"), std::string("node")}, 
    object::pair{std::string("format"), std::string("cjs")}, 
    object::pair{std::string("splitting"), false}, 
    object::pair{std::string("sourcemap"), true}, 
    object::pair{std::string("external"), array<string>{ std::string("sharp"), std::string("@napi-rs/canvas"), std::string("face-api.js"), std::string("@tensorflow/tfjs-node"), std::string("@tensorflow-models/mobilenet"), std::string("@mapbox/node-pre-gyp"), std::string("mock-aws-s3"), std::string("aws-sdk"), std::string("nock") }}, 
    object::pair{std::string("naming"), std::string("[name].[ext]")}
};

void Main(void)
{
}

MAIN
