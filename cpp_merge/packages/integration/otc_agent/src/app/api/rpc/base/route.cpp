#include "route.hpp"

any POST(std::shared_ptr<NextRequest> request)
{
    if (!ALCHEMY_API_KEY) {
        auto publicRpc = std:("https://mainnet.base.org");
        try
        {
            auto body = std::async([=]() { request->json(); });
            auto response = std::async([=]() { fetch(publicRpc, object{
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
            console->error(std:("[RPC Proxy] Public RPC error:"), error);
            return NextResponse->json(object{
                object::pair{std:("error"), std:("RPC request failed")}
            }, object{
                object::pair{std:("status"), 502}
            });
        }
    }
    try
    {
        auto body = std::async([=]() { request->json(); });
        auto response = std::async([=]() { fetch(ALCHEMY_BASE_URL, object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(body)}
        }); });
        if (!response->ok) {
            console->error(std:("[RPC Proxy] Alchemy error:"), response->status, response->statusText);
            return NextResponse->json(object{
                object::pair{std:("error"), std:("RPC request failed")}
            }, object{
                object::pair{std:("status"), response->status}
            });
        }
        auto data = std::async([=]() { response->json(); });
        return NextResponse->json(data);
    }
    catch (const any& error)
    {
        console->error(std:("[RPC Proxy] Error:"), error);
        return NextResponse->json(object{
            object::pair{std:("error"), std:("RPC proxy error")}
        }, object{
            object::pair{std:("status"), 500}
        });
    }
};


string ALCHEMY_API_KEY = process->env->ALCHEMY_API_KEY;
string ALCHEMY_BASE_URL = std:("https://base-mainnet.g.alchemy.com/v2/") + ALCHEMY_API_KEY + string_empty;

void Main(void)
{
}

MAIN
