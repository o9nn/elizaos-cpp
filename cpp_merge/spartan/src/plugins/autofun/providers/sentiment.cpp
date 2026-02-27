#include "sentiment.hpp"

std::shared_ptr<Provider> sentimentProvider = object{
    object::pair{std:("name"), std:("AUTOFUN_MARKET_SENTIMENT")}, 
    object::pair{std:("description"), std:("Information about the current autofun market")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto sentimentData = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<Sentiment>>>(std:("sentiments")); }))), (array<any>()));
        if (!sentimentData["length"]) {
            elizaLogger->warn(std:("No sentiment data found"));
            return object{
                object::pair{std:("data"), object{}}, 
                object::pair{std:("values"), object{}}, 
                object::pair{std:("text"), string_empty}
            };
        }
        auto sentiments = std:("Current cryptocurrency market data:");
        auto idx = 1;
        auto& __array1814_2218 = sentimentData;
        for (auto __indx1814_2218 = 0_N; __indx1814_2218 < __array1814_2218->get_length(); __indx1814_2218++)
        {
            auto& sentiment = const_(__array1814_2218)[__indx1814_2218];
            {
                if (!sentiment["occuringTokens"]["length"]) continue;
                sentiments += std:("ENTRY ") + idx + std:("\
TIME: ") + sentiment["timeslot"] + std:("\
TOKEN ANALYSIS:\
");
                for (auto& token : sentiment["occuringTokens"])
                {
                    sentiments += string_empty + token["token"] + std:(" - Sentiment: ") + token["sentiment"] + std:("\
") + token["reason"] + std:("\
");
                }
                sentiments += std:("\
-------------------\
");
                idx++;
            }
        }
        auto data = object{
            object::pair{std:("sentimentData"), std:("sentimentData")}
        };
        auto values = object{};
        auto text = sentiments;
        return object{
            object::pair{std:("data"), std:("data")}, 
            object::pair{std:("values"), std:("values")}, 
            object::pair{std:("text"), std:("text")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
