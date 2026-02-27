#include "price-validator.h"

std::shared_ptr<Promise<std::shared_ptr<PriceValidationResult>>> checkPriceDivergence(string tokenAddress, std::shared_ptr<Chain> chain, double poolPriceUsd)
{
    if (OR((!poolPriceUsd), (poolPriceUsd <= 0))) {
        return object{
            object::pair{std:("valid"), true}
        };
    }
    auto platformId = const_(COINGECKO_CHAIN_MAP)[chain];
    if (!platformId) {
        return object{
            object::pair{std:("valid"), true}
        };
    }
    try
    {
        auto url = std:("https://api.coingecko.com/api/v3/simple/token_price/") + platformId + std:("?contract_addresses=") + tokenAddress + std:("&vs_currencies=usd");
        auto cacheKey = std:("coingecko:") + platformId + std:(":") + tokenAddress->toLowerCase() + string_empty;
        auto data = std::async([=]() { fetchJsonWithRetryAndCache<std::shared_ptr<CoinGeckoPriceResponse>>(url, object{
            object::pair{std:("headers"), object{
                object::pair{std:("Accept"), std:("application/json")}
            }}
        }, object{
            object::pair{std:("cacheTtlMs"), COINGECKO_CACHE_TTL_MS}, 
            object::pair{std:("cacheKey"), std:("cacheKey")}, 
            object::pair{std:("maxRetries"), 3}
        }); });
        auto tokenData = (*const_(data))[tokenAddress->toLowerCase()];
        if (OR((!tokenData), (!tokenData["usd"]))) {
            return object{
                object::pair{std:("valid"), true}
            };
        }
        auto aggregatedPrice = tokenData["usd"];
        auto diff = Math->abs(poolPriceUsd - aggregatedPrice);
        auto divergence = diff / aggregatedPrice;
        auto divergencePercent = divergence * 100;
        if (divergencePercent > 10) {
            return object{
                object::pair{std:("valid"), false}, 
                object::pair{std:("warning"), std:("Price Warning: Pool price ($") + poolPriceUsd->toFixed(4) + std:(") diverges by ") + divergencePercent->toFixed(1) + std:("% from market price ($") + aggregatedPrice->toFixed(4) + std:(").")}, 
                object::pair{std:("aggregatedPrice"), std:("aggregatedPrice")}, 
                object::pair{std:("poolPrice"), poolPriceUsd}, 
                object::pair{std:("divergencePercent"), std:("divergencePercent")}
            };
        }
        return object{
            object::pair{std:("valid"), true}, 
            object::pair{std:("aggregatedPrice"), std:("aggregatedPrice")}, 
            object::pair{std:("poolPrice"), poolPriceUsd}, 
            object::pair{std:("divergencePercent"), std:("divergencePercent")}
        };
    }
    catch (const any& error)
    {
        console->warn(std:("Price validation failed:"), error);
        return object{
            object::pair{std:("valid"), true}
        };
    }
};


Record<string, string> COINGECKO_CHAIN_MAP = object{
    object::pair{std:("base"), std:("base")}, 
    object::pair{std:("solana"), std:("solana")}, 
    object::pair{std:("bsc"), std:("binance-smart-chain")}, 
    object::pair{std:("ethereum"), std:("ethereum")}
};
double COINGECKO_CACHE_TTL_MS = 30000;

void Main(void)
{
}

MAIN
