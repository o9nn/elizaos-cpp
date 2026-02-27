#include "/home/runner/work/elizaos-cpp/elizaos-cpp/trust_scoreboard/src/pages/api/daoHoldings.h"

any handler(std::shared_ptr<NextApiRequest> req, std::shared_ptr<NextApiResponse> res)
{
    if (req->method != std:("GET")) {
        return res->status(405)->json(object{
            object::pair{std:("error"), std:("Method not allowed")}
        });
    }
    auto url = std:("https://mainnet.helius-rpc.com/?api-key=") + process->env->NEXT_PUBLIC_SOLANA_API + string_empty;
    auto DAO_WALLET = std:("AM84n1iLdxgVTAyENBcLdjXoyvjentTbu5Q6EpKV1PeG");
    try
    {
        auto response = std::async([=]() { fetch(url, object{
            object::pair{std:("method"), std:("POST")}, 
            object::pair{std:("headers"), object{
                object::pair{std:("Content-Type"), std:("application/json")}
            }}, 
            object::pair{std:("body"), JSON->stringify(object{
                object::pair{std:("jsonrpc"), std:("2.0")}, 
                object::pair{std:("id"), std:("my-id")}, 
                object::pair{std:("method"), std:("searchAssets")}, 
                object::pair{std:("params"), object{
                    object::pair{std:("ownerAddress"), DAO_WALLET}, 
                    object::pair{std:("tokenType"), std:("fungible")}, 
                    object::pair{std:("displayOptions"), object{
                        object::pair{std:("showNativeBalance"), true}
                    }}
                }}
            })}
        }); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std:("Failed to fetch from Helius API")));
        }
        auto data = std::async([=]() { response->json(); });
        if (!data->result["items"]) {
            throw any(std::make_shared<Error>(std:("Invalid data format received from API")));
        }
        shared totalValue = data->result["nativeBalance"]["total_price"];
        shared holdings = data->result["items"]->map([=](auto item, auto index) mutable
        {
            auto tokenInfo = item["token_info"];
            auto tokenValue = OR((tokenInfo->price_info["total_price"]), (0));
            auto tokenAmount = OR((item["amount"]), (0));
            auto formattedAmount = formatTokenAmount(tokenAmount, tokenInfo->decimals);
            return object{
                object::pair{std:("rank"), index + 1}, 
                object::pair{std:("name"), OR((tokenInfo->symbol), (tokenInfo->name))}, 
                object::pair{std:("holdings"), string_empty + formattedAmount + std:(" ") + tokenInfo->symbol + string_empty}, 
                object::pair{std:("value"), formatCurrency(tokenValue)}, 
                object::pair{std:("percentage"), calculatePercentage(tokenValue, totalValue)}, 
                object::pair{std:("imageUrl"), std:("/images/tokens/") + (OR((tokenInfo->symbol), (std:("default")))) + std:(".png")}
            };
        }
        );
        return res->status(200)->json(object{
            object::pair{std:("holdings"), std:("holdings")}
        });
    }
    catch (const any& error)
    {
        console->error(std:("Error:"), error);
        return res->status(500)->json(object{
            object::pair{std:("error"), std:("Failed to fetch holdings")}
        });
    }
};


string formatCurrency(double amount)
{
    return ((std::make_shared<Intl::NumberFormat>(std:("en-US"), object{
        object::pair{std:("style"), std:("currency")}, 
        object::pair{std:("currency"), std:("USD")}
    })))->format(amount);
};


string calculatePercentage(double amount, double total)
{
    if (!total) return std:("0%");
    return string_empty + ((amount / total) * 100)->toFixed(2) + std:("%");
};


string formatTokenAmount(double amount, double decimals)
{
    return (amount / Math->pow(10, decimals))->toFixed(4);
};


