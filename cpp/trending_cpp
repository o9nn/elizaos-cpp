#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-coinmarketcap/src/providers/trending.h"

std::shared_ptr<Provider> trendingProvider = object{
    object::pair{std::string("name"), std::string("COINMARKETCAP_CURRENCY_LATEST")}, 
    object::pair{std::string("description"), std::string("Coinmarketcaps latest information about the cryptocurrencies")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto tokens = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<IToken>>>(std::string("coinmarketcap_sync")); }))), (array<any>()));
        if (!tokens["length"]) {
            logger->warn(std::string("No CMC token data found"));
            return false;
        }
        auto latestTxt = std::string("\
Current CoinMarketCap list of all active cryptocurrencies with latest market data:");
        auto idx = 1;
        auto reduceTokens = tokens["map"]([=](auto t) mutable
        {
            auto obj = object{
                object::pair{std::string("name"), t["name"]}, 
                object::pair{std::string("rank"), t["rank"]}, 
                object::pair{std::string("chain"), t["chain"]}, 
                object::pair{std::string("priceUsd"), t["price"]}, 
                object::pair{std::string("symbol"), t["symbol"]}, 
                object::pair{std::string("address"), t["address"]}, 
                object::pair{std::string("volume24hUSD"), t["volume24hUSD"]}, 
                object::pair{std::string("price24hChangePercent"), t["price24hChangePercent"]}
            };
            if (t["liquidity"] != nullptr) obj["liquidity"] = t["liquidity"];
            if (t["marketcap"] != 0) obj["marketcap"] = t["marketcap"];
            return obj;
        }
        );
        latestTxt += std::string("\
") + JSON->stringify(reduceTokens) + std::string("\
");
        auto data = object{
            object::pair{std::string("tokens"), std::string("tokens")}
        };
        auto values = object{};
        auto text = latestTxt + std::string("\
");
        return object{
            object::pair{std::string("data"), std::string("data")}, 
            object::pair{std::string("values"), std::string("values")}, 
            object::pair{std::string("text"), std::string("text")}
        };
        return false;
    }
    }
};

void Main(void)
{
}

MAIN
