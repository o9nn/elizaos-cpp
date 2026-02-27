#include "birdeyeTrending.hpp"

std::shared_ptr<Provider> birdeyeTrendingProvider = object{
    object::pair{std:("name"), std:("BIRDEYE_TRENDING_CRYPTOCURRENCY")}, 
    object::pair{std:("description"), std:("Birdeye trending cryptocurrencies")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
    {
        console->log(std:("intel:provider - get birdeye"));
        auto chains = array<string>{ std:("solana"), std:("base") };
        auto tokens = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<IToken>>>(std:("tokens_solana")); }))), (array<any>()));
        if (!tokens["length"]) {
            logger->warn(std:("intel:provider - no birdeye token data found"));
            return false;
        }
        auto latestTxt = std:("\
Current Birdeye Trending list:");
        auto idx = 1;
        auto reduceTokens = tokens["map"]([=](auto t) mutable
        {
            auto obj = object{
                object::pair{std:("name"), t["name"]}, 
                object::pair{std:("rank"), t["rank"]}, 
                object::pair{std:("chain"), t["chain"]}, 
                object::pair{std:("priceUsd"), t["price"]}, 
                object::pair{std:("symbol"), t["symbol"]}, 
                object::pair{std:("address"), t["address"]}, 
                object::pair{std:("volume24hUSD"), t["volume24hUSD"]}, 
                object::pair{std:("price24hChangePercent"), t["price24hChangePercent"]}
            };
            if (t["liquidity"] != nullptr) obj["liquidity"] = t["liquidity"];
            if (t["marketcap"] != 0) obj["marketcap"] = t["marketcap"];
            return obj;
        }
        );
        latestTxt += std:("\
") + JSON->stringify(reduceTokens) + std:("\
");
        auto data = object{
            object::pair{std:("tokens"), std:("tokens")}
        };
        auto values = object{};
        auto text = latestTxt + std:("\
");
        return object{
            object::pair{std:("data"), std:("data")}, 
            object::pair{std:("values"), std:("values")}, 
            object::pair{std:("text"), std:("text")}
        };
        return false;
    }
    }
};

void Main(void)
{
}

MAIN
