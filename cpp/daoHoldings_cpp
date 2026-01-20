#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/daoHoldings.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std::string("GET")) {
        return res->status(405)->json(object{
            object::pair{std::string("error"), std::string("Method not allowed")}
        });
    }
    auto url = std::string("https://mainnet.helius-rpc.com/?api-key=") + process->env->NEXT_PUBLIC_SOLANA_API + string_empty;
    auto DAO_WALLET = std::string("AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG");
    try
    {
        auto response = std::async([=]() { fetch(url, object{
            object::pair{std::string("method"), std::string("POST")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("Content-Type"), std::string("application/json")}
            }}, 
            object::pair{std::string("body"), JSON->stringify(object{
                object::pair{std::string("jsonrpc"), std::string("2.0")}, 
                object::pair{std::string("id"), std::string("my-id")}, 
                object::pair{std::string("method"), std::string("searchAssets")}, 
                object::pair{std::string("params"), object{
                    object::pair{std::string("ownerAddress"), DAO_WALLET}, 
                    object::pair{std::string("tokenType"), std::string("fungible")}, 
                    object::pair{std::string("displayOptions"), object{
                        object::pair{std::string("showNativeBalance"), true}
                    }}
                }}
            })}
        }); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std::string("Failed to fetch from Helius API")));
        }
        auto data = std::async([=]() { response->json(); });
        if (!data->result["items"]) {
            throw any(std::make_shared<Error>(std::string("Invalid data format received from API")));
        }
        shared totalValue = data->result["nativeBalance"]["total_price"];
        shared holdings = data->result["items"]->map([=](auto item, auto index) mutable
        {
            auto tokenInfo = item["token_info"];
            auto tokenValue = OR((tokenInfo->price_info["total_price"]), (0));
            auto tokenAmount = OR((item["amount"]), (0));
            auto formattedAmount = formatTokenAmount(tokenAmount, tokenInfo->decimals);
            return object{
                object::pair{std::string("rank"), index + 1}, 
                object::pair{std::string("name"), OR((tokenInfo->symbol), (tokenInfo->name))}, 
                object::pair{std::string("holdings"), string_empty + formattedAmount + std::string(" ") + tokenInfo->symbol + string_empty}, 
                object::pair{std::string("value"), formatCurrency(tokenValue)}, 
                object::pair{std::string("percentage"), calculatePercentage(tokenValue, totalValue)}, 
                object::pair{std::string("imageUrl"), std::string("/images/tokens/") + (OR((tokenInfo->symbol), (std::string("default")))) + std::string(".png")}
            };
        }
        );
        return res->status(200)->json(object{
            object::pair{std::string("holdings"), std::string("holdings")}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("Error:"), error);
        return res->status(500)->json(object{
            object::pair{std::string("error"), std::string("Failed to fetch holdings")}
        });
    }
};


string formatCurrency(double amount)
{
    return ((std::make_shared<Intl::NumberFormat>(std::string("en-US"), object{
        object::pair{std::string("style"), std::string("currency")}, 
        object::pair{std::string("currency"), std::string("USD")}
    })))->format(amount);
};


string calculatePercentage(double amount, double total)
{
    if (!total) return std::string("0%");
    return string_empty + ((amount / total) * 100)->toFixed(2) + std::string("%");
};


string formatTokenAmount(double amount, double decimals)
{
    return (amount / Math->pow(10, decimals))->toFixed(4);
};


