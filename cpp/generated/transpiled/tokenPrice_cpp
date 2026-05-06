#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/tokenPrice.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std::string("GET")) {
        return res->status(405)->json(object{
            object::pair{std::string("error"), std::string("Method not allowed")}
        });
    }
    try
    {
        if (!HELIUS_API) {
            throw any(std::make_shared<Error>(std::string("Helius API key not configured")));
        }
        auto response = std::async([=]() { fetch(std::string("https://api.helius.xyz/v0/token-metadata?api-key=") + HELIUS_API + string_empty, object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("mintAccounts"), TOKEN_ADDRESSES}, 
                object::pair{std::string("includeOffChain"), true}, 
                object::pair{std::string("disableCache"), true}
            })}
        }); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std::string("Helius API error: ") + response->status + string_empty));
        }
        auto data = std::async([=]() { response->json(); });
        auto prices = data["map"]([=](auto token) mutable
        {
            return (object{
                object::pair{std::string("address"), token["account"]}, 
                object::pair{std::string("usdPrice"), OR((token["price"]), (0))}
            });
        }
        );
        res->status(200)->json(object{
            object::pair{std::string("prices"), std::string("prices")}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("Token price API error:"), error);
        res->status(200)->json(object{
            object::pair{std::string("prices"), TOKEN_ADDRESSES->map([=](auto address) mutable
            {
                return (object{
                    object::pair{std::string("address"), std::string("address")}, 
                    object::pair{std::string("usdPrice"), 0}
                });
            }
            )}
        });
    }
};


string HELIUS_API = process->env->NEXT_PUBLIC_SOLANA_API;
array<string> TOKEN_ADDRESSES = array<string>{ std::string("So11111111111111111111111111111111111111112"), std::string("HeLp6NuQkmYB4pYWo2zYs22mESHXPQYzXbB8n4V98jwC"), std::string("Gu3LDkn7Vx3bmCzLafYNKcDxv2mH7YN44NJZFXnyai16z"), std::string("7wM4MnbsPsG95A3WhZgbrPWvMtydKmJjqKr2ZVJVpump") };

void Main(void)
{
}

MAIN
