#include "build.config.h"
#include <cstdlib>

std::shared_ptr<BuildConfig> buildConfig = object{
    object::pair{std:("entrypoints"), array<string>{ std:("./src/index.ts") }}, 
    object::pair{std:("outdir"), std:("./dist")}, 
    object::pair{std:("target"), std:("node")}, 
    object::pair{std:("format"), std:("esm")}, 
    object::pair{std:("splitting"), false}, 
    object::pair{std:("sourcemap"), std:("external")}, 
    object::pair{std:("external"), array<string>{ std:("fs"), std:("path"), std:("http"), std:("https"), std:("crypto"), std:("child_process"), std:("os"), std:("util"), std:("stream"), std:("buffer"), std:("events"), std:("url"), std:("node:fs"), std:("node:path"), std:("node:http"), std:("node:https"), std:("node:crypto"), std:("node:child_process"), std:("node:os"), std:("node:util"), std:("node:stream"), std:("node:buffer"), std:("node:events"), std:("node:url"), std:("bun:test"), std:("dotenv"), std:("zod"), std:("@elizaos/core"), std:("@elizaos/plugin-bootstrap"), std:("axios"), std:("sharp"), std:("@napi-rs/canvas"), std:("face-api.js") }}, 
    object::pair{std:("define"), object{
        object::pair{std:("std::getenv("NODE_ENV")"), std:(""production"")}
    }}, 
    object::pair{std:("naming"), std:("[dir]/[name].[ext]")}
};
std::shared_ptr<BuildConfig> workersConfig = object{
    object::pair{std:("entrypoints"), array<string>{ std:("./src/workers/screen-capture-worker.ts"), std:("./src/workers/florence2-worker.ts"), std:("./src/workers/ocr-worker.ts") }}, 
    object::pair{std:("outdir"), std:("./dist/workers")}, 
    object::pair{std:("target"), std:("node")}, 
    object::pair{std:("format"), std:("cjs")}, 
    object::pair{std:("splitting"), false}, 
    object::pair{std:("sourcemap"), true}, 
    object::pair{std:("external"), array<string>{ std:("sharp"), std:("@napi-rs/canvas"), std:("face-api.js"), std:("@tensorflow/tfjs-node"), std:("@tensorflow-models/mobilenet"), std:("@mapbox/node-pre-gyp"), std:("mock-aws-s3"), std:("aws-sdk"), std:("nock") }}, 
    object::pair{std:("naming"), std:("[name].[ext]")}
};

void Main(void)
{
}

MAIN
