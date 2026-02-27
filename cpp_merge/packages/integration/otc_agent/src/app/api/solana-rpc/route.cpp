#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/app/api/solana-rpc/route.h"

any POST(std::shared_ptr<NextRequest> request)
{
    auto heliusKey = process->env->HELIUS_API_KEY;
    if (!heliusKey) {
        return NextResponse->json(object{
            object::pair{std:("error"), std:("Solana RPC not configured")}
        }, object{
            object::pair{std:("status"), 500}
        });
    }
    try
    {
        auto body = std::async([=]() { request->json(); });
        auto response = std::async([=]() { fetch(std:("https://mainnet.helius-rpc.com/?api-key=") + heliusKey + string_empty, object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(body)}
        }); });
        auto data = std::async([=]() { response->json(); });
        return NextResponse->json(data);
    }
    catch (const any& error)
    {
        console->error(std:("[Solana RPC Proxy] Error:"), error);
        return NextResponse->json(object{
            object::pair{std:("error"), std:("RPC request failed")}
        }, object{
            object::pair{std:("status"), 500}
        });
    }
};


