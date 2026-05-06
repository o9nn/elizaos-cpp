#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofun/providers/sentiment.h"

std::shared_ptr<Provider> sentimentProvider = object{
    object::pair{std::string("name"), std::string("AUTOFUN_MARKET_SENTIMENT")}, 
    object::pair{std::string("description"), std::string("Information about the current autofun market")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state) mutable
    {
        auto sentimentData = OR(((std::async([=]() { runtime->getCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); }))), (array<any>()));
        if (!sentimentData["length"]) {
            elizaLogger->warn(std::string("No sentiment data found"));
            return object{
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("values"), object{}}, 
                object::pair{std::string("text"), string_empty}
            };
        }
        auto sentiments = std::string("Current cryptocurrency market data:");
        auto idx = 1;
        auto& __array1814_2218 = sentimentData;
        for (auto __indx1814_2218 = 0_N; __indx1814_2218 < __array1814_2218->get_length(); __indx1814_2218++)
        {
            auto& sentiment = const_(__array1814_2218)[__indx1814_2218];
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
        auto text = sentiments;
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
