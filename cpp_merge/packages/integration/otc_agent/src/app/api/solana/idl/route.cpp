#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/app/api/solana/idl/route.h"

any GET()
{
    auto idlPath = path->join(process->cwd(), std:("solana"), std:("otc-program"), std:("target"), std:("idl"), std:("otc.json"));
    auto data = std::async([=]() { fs::readFile(idlPath, std:("utf8")); });
    return std::make_shared<NextResponse>(data, object{
        object::pair{std:("status"), 200}, 
        object::pair{std:("headers"), object{
            object::pair{std:("content-type"), std:("application/json")}
        }}
    });
};


