#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autonomous-trader/strategies/strategy_llm.h"

void llmStrategy(std::shared_ptr<IAgentRuntime> runtime)
{
    shared service = std::async([=]() { acquireService(runtime, std::string("TRADER_STRATEGY"), std::string("llm trading strategy")); });
    auto infoService = std::async([=]() { acquireService(runtime, std::string("TRADER_DATAPROVIDER"), std::string("llm trading info")); });
    auto me = object{
        object::pair{std::string("name"), std::string("LLM trading strategy")}
    };
    shared hndl = std::async([=]() { service["register_strategy"](me); });
    std::async([=]() { infoService["interested_trending"]([=](auto results) mutable
    {
        console->log(std::string("LLM trading strategy"), results);
        std::async([=]() { generateBuySignal(runtime, service, hndl); });
    }
    ); });
};


void generateBuySignal(any runtime, any strategyService, any hndl)
{
    auto sentimentsData = OR(((std::async([=]() { runtime["getCache"]<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); }))), (array<any>()));
    auto trendingData = OR(((std::async([=]() { runtime["getCache"]<array<std::shared_ptr<IToken>>>(std::string("tokens_solana")); }))), (array<any>()));
    auto sentiments = string_empty;
    auto idx = 1;
    auto& __array3431_3845 = sentimentsData;
    for (auto __indx3431_3845 = 0_N; __indx3431_3845 < __array3431_3845->get_length(); __indx3431_3845++)
    {
        auto& sentiment = const_(__array3431_3845)[__indx3431_3845];
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
    auto tokens = string_empty;
    if (!trendingData["length"]) {
        logger->warn(std::string("No trending tokens found in cache"));
    } else {
        auto index = 1;
        for (auto& token : trendingData)
        {
            tokens += std::string("ENTRY ") + index + std::string("\
\
TOKEN SYMBOL: ") + token["name"] + std::string("\
TOKEN ADDRESS: ") + token["address"] + std::string("\
PRICE: ") + token["price"] + std::string("\
24H CHANGE: ") + token["price24hChangePercent"] + std::string("\
LIQUIDITY: ") + token["liquidity"] + string_empty;
            tokens += std::string("\
-------------------\
");
            index++;
        }
    }
    auto prompt = buyTemplate->replace(std::string("{{sentiment}}"), sentiments)->replace(std::string("{{trending_tokens}}"), tokens);
    auto requiredFields = array<string>{ std::string("recommend_buy"), std::string("reason"), std::string("recommend_buy_address") };
    auto response = std::async([=]() { askLlmObject(runtime, object{
        object::pair{std::string("prompt"), std::string("prompt")}, 
        object::pair{std::string("system"), std::string("You are a buy signal analyzer.")}
    }, requiredFields); });
    console->log(std::string("response"), response);
    if (response["recommend_buy_chain"] != std::string("solana")) {
        return std::shared_ptr<Promise<void>>();
    }
    auto solanaService = std::async([=]() { acquireService(runtime, std::string("chain_solana"), std::string("llm trading strategy")); });
    if (!solanaService["validateAddress"](response["recommend_buy_address"])) {
    }
    auto infoService = std::async([=]() { acquireService(runtime, std::string("TRADER_DATAPROVIDER"), std::string("llm trading info")); });
    auto token = std::async([=]() { infoService["getToken"](response["recommend_buy_chain"], response["recommend_buy_address"]); });
    auto scaleAmount = [=](auto walletKeypair, auto balance, auto signal) mutable
    {
    };

};


void onPriceDelta()
{
};


void onSentimentDelta()
{
};


void onVol24hDelta()
{
};


void onLiquidDelta()
{
};


string buyTemplate = std::string("\
I want you to give a crypto buy signal based on both the sentiment analysis as well as the trending tokens.\
Only choose a token that occurs in both the Trending Tokens list as well as the Sentiment analysis. This ensures we have the proper symbol, chain and token address.\
The sentiment score has a range of -100 to 100, with -100 indicating extreme negativity and 100 indicating extreme positiveness.\
Please determine how good of an opportunity this is (how rare and how much potential).\
Also let me know what a good amount would be to buy. Buy amount should be a range between 1 and 99% of available balance.\
if no sentiment or trending, it's ok to use your feelings instead of your mind.\
\
Sentiment analysis:\
\
{{sentiment}}\
\
Trending tokens:\
\
{{trending_tokens}}\
\
Only return the following JSON and nothing else (even if no sentiment or trending):\
{\
  recommend_buy: "the symbol of the token for example DEGENAI. can use NULL if nothing strikes you",\
  recommend_buy_chain: "which chain the token is on",\
  recommend_buy_address: "the address of the token to purchase, for example: Gu3LDkn7Vx3bmCzLafYNKcDxv2mH7YN44NJZFXnypump",\
  reason: "the reason why you think this is a good buy, and why you chose the specific amount",\
  opportunity_score: "number, for example 50",\
  buy_amount: "number between 1 and 100, for example: 23",\
  exit_conditions: "what conditions in which you'd change your position on this token",\
  exit_sentiment_drop_threshold: "what drop in sentiment in which you'd change your position on this token",\
  exit_24hvolume_threshold: "what drop in 24h volume in which you'd change your position on this token",\
  exit_price_drop_threshold: "what drop in price in which you'd change your position on this token",\
  exit_target_price: "what target price do you think we should get out of the position at",\
}");

void Main(void)
{
}

MAIN
