#include "birdeye.hpp"

BirdeyeService::BirdeyeService(string apiKey_) : apiKey(apiKey_)  {
}

any BirdeyeService::getBirdeyeFetchOptions()
{
    return object{
        object::pair{std:("headers"), object{
            object::pair{std:("accept"), std:("application/json")}, 
            object::pair{std:("x-CHAIN"), std:("solana")}, 
            object::pair{std:("X-API-KEY"), this->apiKey}
        }}
    };
}

std::shared_ptr<Promise<object>> BirdeyeService::getTokenMarketData(string tokenAddress)
{
    try
    {
        if (tokenAddress == std:("So11111111111111111111111111111111111111111")) {
            tokenAddress = std:("So11111111111111111111111111111111111111112");
        }
        auto [response, volResponse, priceHistoryResponse] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<std::shared_ptr<Response>>>, std::shared_ptr<Promise<std::shared_ptr<Response>>>, std::shared_ptr<Promise<std::shared_ptr<Response>>>>{ fetch(string_empty + PROVIDER_CONFIG["BIRDEYE_API"] + std:("/defi/v3/token/market-data?address=") + tokenAddress + string_empty, this->getBirdeyeFetchOptions()), fetch(string_empty + PROVIDER_CONFIG["BIRDEYE_API"] + std:("/defi/price_volume/single?address=") + tokenAddress + std:("&type=24h"), this->getBirdeyeFetchOptions()), fetch(string_empty + PROVIDER_CONFIG["BIRDEYE_API"] + std:("/defi/history_price?address=") + tokenAddress + std:("&address_type=token&type=15m"), this->getBirdeyeFetchOptions()) }); });
        if (OR((OR((!response->ok), (!volResponse->ok))), (!priceHistoryResponse->ok))) {
            throw any(std::make_shared<Error>(std:("Birdeye API error for token ") + tokenAddress + string_empty));
        }
        auto [data, volData, priceHistoryData] = std::async([=]() { Promise->all(std::tuple<std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>, std::shared_ptr<Promise<any>>>{ response->json(), volResponse->json(), priceHistoryResponse->json() }); });
        if (!data["data"]) {
            logger->warn(std:("getTokenMarketData - cant save result"), data, std:("for"), tokenAddress);
            return this->getEmptyMarketData();
        }
        return object{
            object::pair{std:("price"), data["data"]["price"]}, 
            object::pair{std:("marketCap"), OR((data["data"]["market_cap"]), (0))}, 
            object::pair{std:("liquidity"), OR((data["data"]["liquidity"]), (0))}, 
            object::pair{std:("volume24h"), OR((volData["data"]["volumeUSD"]), (0))}, 
            object::pair{std:("priceHistory"), priceHistoryData["data"]["items"]["map"]([=](auto item) mutable
            {
                return item["value"];
            }
            )}
        };
    }
    catch (const any& error)
    {
        logger->error(std:("Error fetching token market data:"), error);
        return this->getEmptyMarketData();
    }
}

std::shared_ptr<Promise<any>> BirdeyeService::getTokensMarketData(array<string> tokenAddresses)
{
    auto tokenDb = object{};
    try
    {
        auto chunkArray = [=](auto arr, auto size) mutable
        {
            return arr->map([=](auto _, auto i) mutable
            {
                return ((i % size == 0) ? any(arr->slice(i, i + size)) (nullptr));
            }
            )->filter(Boolean);
        };
        auto hundos = chunkArray(tokenAddresses, 100);
        auto multipricePs = hundos->map([=](auto addresses) mutable
        {
            auto listStr = addresses->join(std:(","));
            return fetch(string_empty + PROVIDER_CONFIG["BIRDEYE_API"] + std:("/defi/multi_price?list_address=") + listStr + std:("&include_liquidity=true"), this->getBirdeyeFetchOptions());
        }
        );
        auto multipriceResps = std::async([=]() { Promise->all(multipricePs); });
        auto multipriceData = std::async([=]() { Promise->all(multipriceResps->map([=](auto resp) mutable
        {
            return resp->json();
        }
        )); });
        for (auto& mpd : multipriceData)
        {
            for (auto& ca : keys_(mpd["data"]))
            {
                auto t = const_(mpd["data"])[ca];
                if (t) {
                    tokenDb[ca] = object{
                        object::pair{std:("priceUsd"), t["value"]}, 
                        object::pair{std:("priceSol"), t["priceInNative"]}, 
                        object::pair{std:("liquidity"), t["liquidity"]}, 
                        object::pair{std:("priceChange24h"), t["priceChange24h"]}
                    };
                } else {
                    logger->warn(ca, std:("mpd error"), t);
                }
            }
        }
        return tokenDb;
    }
    catch (const any& error)
    {
        logger->error(std:("Error fetching multiple tokens market data:"), error);
        return tokenDb;
    }
}

any BirdeyeService::getEmptyMarketData()
{
    return object{
        object::pair{std:("price"), 0}, 
        object::pair{std:("marketCap"), 0}, 
        object::pair{std:("liquidity"), 0}, 
        object::pair{std:("volume24h"), 0}, 
        object::pair{std:("priceHistory"), array<any>()}
    };
}

