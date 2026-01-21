#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/utils/price-validator.h"

std::shared_ptr<Promise<std::shared_ptr<PriceValidationResult>>> checkPriceDivergence(string tokenAddress, std::shared_ptr<Chain> chain, double poolPriceUsd)
{
    if (OR((!poolPriceUsd), (poolPriceUsd <= 0))) {
        return object{
            object::pair{std::string("valid"), true}
        };
    }
    auto platformId = const_(COINGECKO_CHAIN_MAP)[chain];
    if (!platformId) {
        return object{
            object::pair{std::string("valid"), true}
        };
    }
    try
    {
        auto url = std::string("https://api.coingecko.com/api/v3/simple/token_price/") + platformId + std::string("?contract_addresses=") + tokenAddress + std::string("&vs_currencies=usd");
        auto cacheKey = std::string("coingecko:") + platformId + std::string(":") + tokenAddress->toLowerCase() + string_empty;
        auto data = std::async([=]() { fetchJsonWithRetryAndCache<std::shared_ptr<CoinGeckoPriceResponse>>(url, object{
            object::pair{std::string("headers"), object{
                object::pair{std::string("Accept"), std::string("application/json")}
            }}
        }, object{
            object::pair{std::string("cacheTtlMs"), COINGECKO_CACHE_TTL_MS}, 
            object::pair{std::string("cacheKey"), std::string("cacheKey")}, 
            object::pair{std::string("maxRetries"), 3}
        }); });
        auto tokenData = (*const_(data))[tokenAddress->toLowerCase()];
        if (OR((!tokenData), (!tokenData["usd"]))) {
            return object{
                object::pair{std::string("valid"), true}
            };
        }
        auto aggregatedPrice = tokenData["usd"];
        auto diff = Math->abs(poolPriceUsd - aggregatedPrice);
        auto divergence = diff / aggregatedPrice;
        auto divergencePercent = divergence * 100;
        if (divergencePercent > 10) {
            return object{
                object::pair{std::string("valid"), false}, 
                object::pair{std::string("warning"), std::string("Price Warning: Pool price ($") + poolPriceUsd->toFixed(4) + std::string(") diverges by ") + divergencePercent->toFixed(1) + std::string("% from market price ($") + aggregatedPrice->toFixed(4) + std::string(").")}, 
                object::pair{std::string("aggregatedPrice"), std::string("aggregatedPrice")}, 
                object::pair{std::string("poolPrice"), poolPriceUsd}, 
                object::pair{std::string("divergencePercent"), std::string("divergencePercent")}
            };
        }
        return object{
            object::pair{std::string("valid"), true}, 
            object::pair{std::string("aggregatedPrice"), std::string("aggregatedPrice")}, 
            object::pair{std::string("poolPrice"), poolPriceUsd}, 
            object::pair{std::string("divergencePercent"), std::string("divergencePercent")}
        };
    }
    catch (const any& error)
    {
        console->warn(std::string("Price validation failed:"), error);
        return object{
            object::pair{std::string("valid"), true}
        };
    }
};


Record<string, string> COINGECKO_CHAIN_MAP = object{
    object::pair{std::string("base"), std::string("base")}, 
    object::pair{std::string("solana"), std::string("solana")}, 
    object::pair{std::string("bsc"), std::string("binance-smart-chain")}, 
    object::pair{std::string("ethereum"), std::string("ethereum")}
};
double COINGECKO_CACHE_TTL_MS = 30000;

void Main(void)
{
}

MAIN
