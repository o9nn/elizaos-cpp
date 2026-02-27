#include "sentiment.hpp"

std::shared_ptr<Provider> sentimentProvider = object{
    object::pair{std:("name"), std:("CRYPTOTWITTER_MARKET_SENTIMENT")}, 
    object::pair{std:("description"), std:("Information about the current cryptocurrency twitter sentiment")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto sentimentData = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<Sentiment>>>(std:("sentiments")); }))), (array<any>()));
        if (!sentimentData["length"]) {
            logger->warn(std:("No sentiment data found"));
            return false;
        }
        auto sentiments = std:("\
Current cryptocurrency market data:");
        auto idx = 1;
        auto& __array1577_1981 = sentimentData;
        for (auto __indx1577_1981 = 0_N; __indx1577_1981 < __array1577_1981->get_length(); __indx1577_1981++)
        {
            auto& sentiment = const_(__array1577_1981)[__indx1577_1981];
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
        auto text = sentiments + std:("\
");
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
