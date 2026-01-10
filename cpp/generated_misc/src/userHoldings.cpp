#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/userHoldings.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std::string("GET")) {
        return res->status(405)->json(object{
            object::pair{std::string("error"), std::string("Method not allowed")}
        });
    }
    auto walletAddress = as<string>(req->query->wallet);
    if (!walletAddress) {
        return res->status(400)->json(object{
            object::pair{std::string("error"), std::string("Wallet address is required")}
        });
    }
    try
    {
        console->log(std::string("Fetching holdings for wallet:"), walletAddress);
        auto response = std::async([=]() { fetch(HELIUS_API, object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("jsonrpc"), std::string("2.0")}, 
                object::pair{std::string("id"), std::string("my-id")}, 
                object::pair{std::string("method"), std::string("searchAssets")}, 
                object::pair{std::string("params"), object{
                    object::pair{std::string("ownerAddress"), walletAddress}, 
                    object::pair{std::string("tokenType"), std::string("fungible")}, 
                    object::pair{std::string("displayOptions"), object{
                        object::pair{std::string("showNativeBalance"), true}
                    }}
                }}
            })}
        }); });
        auto data = std::async([=]() { response->json(); });
        if (OR((!response->ok), (data["error"]))) {
            throw any(std::make_shared<Error>(OR((data["error"]["message"]), (std::string("Failed to fetch token data")))));
        }
        if (!data["result"]["items"]) {
            return res->status(200)->json(object{
                object::pair{std::string("holdings"), array<any>()}
            });
        }
        auto holdings = data["result"]["items"]["map"]([=](auto item) mutable
        {
            auto tokenInfo = OR((item["token_info"]), (object{}));
            auto decimals = (tokenInfo["decimals"] != undefined) ? any(tokenInfo["decimals"]) : any(9);
            auto rawAmount = OR((OR((tokenInfo["balance"]), (tokenInfo["amount"]))), (0));
            auto amount = Number(rawAmount) / Math->pow(10, decimals);
            auto price = OR((tokenInfo["price_info"]["price_per_token"]), (0));
            auto value = amount * price;
            return object{
                object::pair{std::string("name"), OR((OR((tokenInfo["symbol"]), (tokenInfo["name"]))), (item["id"]))}, 
                object::pair{std::string("amount"), std::string("amount")}, 
                object::pair{std::string("price"), std::string("price")}, 
                object::pair{std::string("value"), std::string("value")}, 
                object::pair{std::string("allocation"), 0}
            };
        }
        )["filter"]([=](auto holding) mutable
        {
            return holding["amount"] > 0;
        }
        );
        if (holdings["length"] == 0) {
            console->log(std::string("No holdings found after processing."));
        }
        shared totalValue = holdings["reduce"]([=](auto sum, auto h) mutable
        {
            return sum + h["value"];
        }
        , 0);
        holdings["forEach"]([=](auto h) mutable
        {
            h["allocation"] = (totalValue > 0) ? any((h["value"] / totalValue) * 100) : any(0);
        }
        );
        holdings["sort"]([=](auto a, auto b) mutable
        {
            return b["value"] - a["value"];
        }
        );
        return res->status(200)->json(object{
            object::pair{std::string("holdings"), std::string("holdings")}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("API error:"), error);
        return res->status(500)->json(object{
            object::pair{std::string("error"), (is<Error>(error)) ? any(error->message) : any(std::string("Failed to fetch holdings"))}
        });
    }
};


string HELIUS_API = std::string("https://mainnet.helius-rpc.com/?api-key=") + process->env->NEXT_PUBLIC_SOLANA_API + string_empty;

void Main(void)
{
}

MAIN
