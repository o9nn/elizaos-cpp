#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/providers/birdeyeTrending.h"

std::shared_ptr<Provider> birdeyeTrendingProvider = object{
    object::pair{std::string("name"), std::string("BIRDEYE_TRENDING_CRYPTOCURRENCY")}, 
    object::pair{std::string("description"), std::string("Birdeye trending cryptocurrencies")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        console->log(std::string("intel:provider - get birdeye"));
        auto chains = array<string>{ std::string("solana"), std::string("base") };
        auto tokens = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<IToken>>>(std::string("tokens_solana")); }))), (array<any>()));
        if (!tokens["length"]) {
            logger->warn(std::string("intel:provider - no birdeye token data found"));
            return false;
        }
        auto latestTxt = std::string("\
Current Birdeye Trending list:");
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
