#include "route.hpp"
#include <string>

any POST(std::shared_ptr<NextRequest> request)
{
    auto heliusKey = process->env->HELIUS_API_KEY;
    if (!heliusKey) {
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("Solana RPC not configured")}
        }, object{
            object::pair{std::string("status"), 500}
        });
    }
    try
    {
        auto body = std::async([=]() { request->json(); });
        auto response = std::async([=]() { fetch(std::string("https://mainnet.helius-rpc.com/?api-key=") + heliusKey + string_empty, object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(body)}
        }); });
        auto data = std::async([=]() { response->json(); });
        return NextResponse->json(data);
    }
    catch (const any& error)
    {
        console->error(std::string("[Solana RPC Proxy] Error:"), error);
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("RPC request failed")}
        }, object{
            object::pair{std::string("status"), 500}
        });
    }
};


