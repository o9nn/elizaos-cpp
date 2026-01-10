#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/app/api/solana/idl/route.h"

any GET()
{
    auto idlPath = path->join(process->cwd(), std::string("solana"), std::string("otc-program"), std::string("target"), std::string("idl"), std::string("otc.json"));
    auto data = std::async([=]() { fs::readFile(idlPath, std::string("utf8")); });
    return std::make_shared<NextResponse>(data, object{
        object::pair{std::string("status"), 200}, 
        object::pair{std::string("headers"), object{
            object::pair{std::string("content-type"), std::string("application/json")}
        }}
    });
};


