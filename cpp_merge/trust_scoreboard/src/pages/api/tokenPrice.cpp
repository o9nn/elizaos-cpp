#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/tokenPrice.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std:("GET")) {
        return res->status(405)->json(object{
            object::pair{std:("error"), std:("Method not allowed")}
        });
    }
    try
    {
        if (!HELIUS_API) {
            throw any(std::make_shared<Error>(std:("Helius API key not configured")));
        }
        auto response = std::async([=]() { fetch(std:("https://api.helius.xyz/v0/token-metadata?api-key=") + HELIUS_API + string_empty, object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("mintAccounts"), TOKEN_ADDRESSES}, 
                object::pair{std:("includeOffChain"), true}, 
                object::pair{std:("disableCache"), true}
            })}
        }); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std:("Helius API error: ") + response->status + string_empty));
        }
        auto data = std::async([=]() { response->json(); });
        auto prices = data["map"]([=](auto token) mutable
        {
            return (object{
                object::pair{std:("address"), token["account"]}, 
                object::pair{std:("usdPrice"), OR((token["price"]), (0))}
            });
        }
        );
        res->status(200)->json(object{
            object::pair{std:("prices"), std:("prices")}
        });
    }
    catch (const any& error)
    {
        console->error(std:("Token price API error:"), error);
        res->status(200)->json(object{
            object::pair{std:("prices"), TOKEN_ADDRESSES->map([=](auto address) mutable
            {
                return (object{
                    object::pair{std:("address"), std:("address")}, 
                    object::pair{std:("usdPrice"), 0}
                });
            }
            )}
        });
    }
};


string HELIUS_API = process->env->NEXT_PUBLIC_SOLANA_API;
array<string> TOKEN_ADDRESSES = array<string>{ std:("So11111111111111111111111111111111111111112"), std:("HeLp6NuQkmYB4pYWo2zYs22mESHXPQYzXbB8n4V98jwC"), std:("Gu3LDkn7Vx3bmCzLafYNKcDxv2mH7YN44NJZFXnyai16z"), std:("7wM4MnbsPsG95A3WhZgbrPWvMtydKmJjqKr2ZVJVpump") };

void Main(void)
{
}

MAIN
