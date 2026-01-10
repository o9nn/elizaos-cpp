#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/providers/sentiment.h"

std::shared_ptr<Provider> sentimentProvider = object{
    object::pair{std::string("name"), std::string("CRYPTOTWITTER_MARKET_SENTIMENT")}, 
    object::pair{std::string("description"), std::string("Information about the current cryptocurrency twitter sentiment")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto sentimentData = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); }))), (array<any>()));
        if (!sentimentData["length"]) {
            logger->warn(std::string("No sentiment data found"));
            return false;
        }
        auto sentiments = std::string("\
Current cryptocurrency market data:");
        auto idx = 1;
        auto& __array1577_1981 = sentimentData;
        for (auto __indx1577_1981 = 0_N; __indx1577_1981 < __array1577_1981->get_length(); __indx1577_1981++)
        {
            auto& sentiment = const_(__array1577_1981)[__indx1577_1981];
            {
                if (!sentiment["occuringTokens"]["length"]) continue;
                sentiments += std::string("ENTRY ") + idx + std::string("\
TIME: ") + sentiment["timeslot"] + std::string("\
TOKEN ANALYSIS:\
");
                for (auto& token : sentiment["occuringTokens"])
                {
                    sentiments += string_empty + token["token"] + std::string(" - Sentiment: ") + token["sentiment"] + std::string("\
") + token["reason"] + std::string("\
");
                }
                sentiments += std::string("\
-------------------\
");
                idx++;
            }
        }
        auto data = object{
            object::pair{std::string("sentimentData"), std::string("sentimentData")}
        };
        auto values = object{};
        auto text = sentiments + std::string("\
");
        return object{
            object::pair{std::string("data"), std::string("data")}, 
            object::pair{std::string("values"), std::string("values")}, 
            object::pair{std::string("text"), std::string("text")}
        };
    }
    }
};

void Main(void)
{
}

MAIN
