#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofun/providers/autofun.h"

std::shared_ptr<Provider> autofunProvider = object{
    object::pair{std::string("name"), std::string("AUTOFUN_INFORMATION")}, 
    object::pair{std::string("description"), std::string("Autofun latest information about the cryptocurrencies on it's platform")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto url = std::string("https://api.auto.fun/api/tokens?limit=200&page=1&sortBy=createdAt&sortOrder=desc&hideImported=1");
        auto res = std::async([=]() { fetch(url); });
        auto tokens = std::async([=]() { res->json(); });
        console->log(std::string("autofun data"), tokens["length"]);
        auto latestTxt = std::string("\
Current Auto.fun list of all active cryptocurrencies with latest market data:\
");
        auto idx = 1;
        auto fields = array<string>{ std::string("id"), std::string("name"), std::string("ticker"), std::string("url"), std::string("twitter"), std::string("telegram"), std::string("discord"), std::string("farcaster"), std::string("description"), std::string("liquidity"), std::string("currentPrice"), std::string("tokenSupplyUiAmount"), std::string("holderCount"), std::string("volume24h"), std::string("price24hAgo"), std::string("priceChange24h"), std::string("curveProgress") };
        auto remaps = object{
            object::pair{std::string("ticker"), std::string("symbol")}
        };
        latestTxt += std::string("id, name, symbol, url, twitter, telegram, discord, farcaster, description, liquidity, currentPrice, tokenSupplyUiAmount, holderCount, volume24h, price24hAgo, priceChange24h, curveProgress");
        for (auto& t : tokens)
        {
            auto out = array<any>();
            for (auto& f : fields)
            {
                out->push(const_(t)[f]);
            }
            latestTxt += out->join(std::string(", ")) + std::string("\
");
        }
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
