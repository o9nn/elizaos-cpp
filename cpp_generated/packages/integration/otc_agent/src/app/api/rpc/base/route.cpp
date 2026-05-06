#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/app/api/rpc/base/route.h"

any POST(std::shared_ptr<NextRequest> request)
{
    if (!ALCHEMY_API_KEY) {
        auto publicRpc = std::string("https://mainnet.base.org");
        try
        {
            auto body = std::async([=]() { request->json(); });
            auto response = std::async([=]() { fetch(publicRpc, object{
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
            console->error(std::string("[RPC Proxy] Public RPC error:"), error);
            return NextResponse->json(object{
                object::pair{std::string("error"), std::string("RPC request failed")}
            }, object{
                object::pair{std::string("status"), 502}
            });
        }
    }
    try
    {
        auto body = std::async([=]() { request->json(); });
        auto response = std::async([=]() { fetch(ALCHEMY_BASE_URL, object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(body)}
        }); });
        if (!response->ok) {
            console->error(std::string("[RPC Proxy] Alchemy error:"), response->status, response->statusText);
            return NextResponse->json(object{
                object::pair{std::string("error"), std::string("RPC request failed")}
            }, object{
                object::pair{std::string("status"), response->status}
            });
        }
        auto data = std::async([=]() { response->json(); });
        return NextResponse->json(data);
    }
    catch (const any& error)
    {
        console->error(std::string("[RPC Proxy] Error:"), error);
        return NextResponse->json(object{
            object::pair{std::string("error"), std::string("RPC proxy error")}
        }, object{
            object::pair{std::string("status"), 500}
        });
    }
};


string ALCHEMY_API_KEY = process->env->ALCHEMY_API_KEY;
string ALCHEMY_BASE_URL = std::string("https://base-mainnet.g.alchemy.com/v2/") + ALCHEMY_API_KEY + string_empty;

void Main(void)
{
}

MAIN
