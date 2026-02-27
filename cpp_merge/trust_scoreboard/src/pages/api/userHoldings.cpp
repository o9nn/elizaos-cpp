#include "userHoldings.hpp"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std:("GET")) {
        return res->status(405)->json(object{
            object::pair{std:("error"), std:("Method not allowed")}
        });
    }
    auto walletAddress = as<string>(req->query->wallet);
    if (!walletAddress) {
        return res->status(400)->json(object{
            object::pair{std:("error"), std:("Wallet address is required")}
        });
    }
    try
    {
        console->log(std:("Fetching holdings for wallet:"), walletAddress);
        auto response = std::async([=]() { fetch(HELIUS_API, object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("jsonrpc"), std:("2.0")}, 
                object::pair{std:("id"), std:("my-id")}, 
                object::pair{std:("method"), std:("searchAssets")}, 
                object::pair{std:("params"), object{
                    object::pair{std:("ownerAddress"), walletAddress}, 
                    object::pair{std:("tokenType"), std:("fungible")}, 
                    object::pair{std:("displayOptions"), object{
                        object::pair{std:("showNativeBalance"), true}
                    }}
                }}
            })}
        }); });
        auto data = std::async([=]() { response->json(); });
        if (OR((!response->ok), (data["error"]))) {
            throw any(std::make_shared<Error>(OR((data["error"]["message"]), (std:("Failed to fetch token data")))));
        }
        if (!data["result"]["items"]) {
            return res->status(200)->json(object{
                object::pair{std:("holdings"), array<any>()}
            });
        }
        auto holdings = data["result"]["items"]["map"]([=](auto item) mutable
        {
            auto tokenInfo = OR((item["token_info"]), (object{}));
            auto decimals = (tokenInfo["decimals"] != undefined) ? any(tokenInfo["decimals"]) (9);
            auto rawAmount = OR((OR((tokenInfo["balance"]), (tokenInfo["amount"]))), (0));
            auto amount = Number(rawAmount) / Math->pow(10, decimals);
            auto price = OR((tokenInfo["price_info"]["price_per_token"]), (0));
            auto value = amount * price;
            return object{
                object::pair{std:("name"), OR((OR((tokenInfo["symbol"]), (tokenInfo["name"]))), (item["id"]))}, 
                object::pair{std:("amount"), std:("amount")}, 
                object::pair{std:("price"), std:("price")}, 
                object::pair{std:("value"), std:("value")}, 
                object::pair{std:("allocation"), 0}
            };
        }
        )["filter"]([=](auto holding) mutable
        {
            return holding["amount"] > 0;
        }
        );
        if (holdings["length"] == 0) {
            console->log(std:("No holdings found after processing."));
        }
        shared totalValue = holdings["reduce"]([=](auto sum, auto h) mutable
        {
            return sum + h["value"];
        }
        , 0);
        holdings["forEach"]([=](auto h) mutable
        {
            h["allocation"] = (totalValue > 0) ? any((h["value"] / totalValue) * 100) (0);
        }
        );
        holdings["sort"]([=](auto a, auto b) mutable
        {
            return b["value"] - a["value"];
        }
        );
        return res->status(200)->json(object{
            object::pair{std:("holdings"), std:("holdings")}
        });
    }
    catch (const any& error)
    {
        console->error(std:("API error:"), error);
        return res->status(500)->json(object{
            object::pair{std:("error"), (is<Error>(error)) ? any(error->message) (std:("Failed to fetch holdings"))}
        });
    }
};


string HELIUS_API = std:("https://mainnet.helius-rpc.com/?api-key=") + process->env->NEXT_PUBLIC_SOLANA_API + string_empty;

void Main(void)
{
}

MAIN
