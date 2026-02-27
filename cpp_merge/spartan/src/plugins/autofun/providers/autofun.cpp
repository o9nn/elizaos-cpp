#include "autofun.hpp"

std::shared_ptr<Provider> autofunProvider = object{
    object::pair{std:("name"), std:("AUTOFUN_INFORMATION")}, 
    object::pair{std:("description"), std:("Autofun latest information about the cryptocurrencies on it's platform")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto url = std:("https://api.auto.fun/api/tokens?limit=200&page=1&sortBy=createdAt&sortOrder=desc&hideImported=1");
        auto res = std::async([=]() { fetch(url); });
        auto tokens = std::async([=]() { res->json(); });
        console->log(std:("autofun data"), tokens["length"]);
        auto latestTxt = std:("\
Current Auto.fun list of all active cryptocurrencies with latest market data:\
");
        auto idx = 1;
        auto fields = array<string>{ std:("id"), std:("name"), std:("ticker"), std:("url"), std:("twitter"), std:("telegram"), std:("discord"), std:("farcaster"), std:("description"), std:("liquidity"), std:("currentPrice"), std:("tokenSupplyUiAmount"), std:("holderCount"), std:("volume24h"), std:("price24hAgo"), std:("priceChange24h"), std:("curveProgress") };
        auto remaps = object{
            object::pair{std:("ticker"), std:("symbol")}
        };
        latestTxt += std:("id, name, symbol, url, twitter, telegram, discord, farcaster, description, liquidity, currentPrice, tokenSupplyUiAmount, holderCount, volume24h, price24hAgo, priceChange24h, curveProgress");
        for (auto& t : tokens)
        {
            auto out = array<any>();
            for (auto& f : fields)
            {
                out->push(const_(t)[f]);
            }
            latestTxt += out->join(std:(", ")) + std:("\
");
        }
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
