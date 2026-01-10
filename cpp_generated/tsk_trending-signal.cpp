#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/tasks/tsk_trending-signal.h"

BuySignal::BuySignal(std::shared_ptr<IAgentRuntime> runtime) {
    this->runtime = runtime;
}

std::shared_ptr<Promise<boolean>> BuySignal::generateSignal()
{
    logger->info(std::string("buy-signal::generateSignal - Updating latest buy signal"));
    auto sentimentsData = OR(((std::async([=]() { this->runtime->getCache<array<std::shared_ptr<Sentiment>>>(std::string("sentiments")); }))), (array<any>()));
    auto sentiments = string_empty;
    auto idx = 1;
    auto& __array3067_3473 = sentimentsData;
    for (auto __indx3067_3473 = 0_N; __indx3067_3473 < __array3067_3473->get_length(); __indx3067_3473++)
    {
        auto& sentiment = const_(__array3067_3473)[__indx3067_3473];
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
    auto prompt = _template->replace(std::string("{{sentiment}}"), sentiments);
    auto tokens = string_empty;
    auto trendingData = OR(((std::async([=]() { this->runtime->getCache<array<std::shared_ptr<IToken>>>(std::string("tokens_solana")); }))), (array<any>()));
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
    auto solanaBalance = std::async([=]() { this->getBalance(); });
    auto finalPrompt = prompt->replace(std::string("{{trending_tokens}}"), tokens)->replace(std::string("{{solana_balance}}"), String(solanaBalance));
    auto responseContent = nullptr;
    auto retries = 0;
    auto maxRetries = 3;
    while (AND((retries < maxRetries), ((OR((OR((!responseContent->recommended_buy), (!responseContent->reason))), (!responseContent->recommend_buy_address))))))
    {
        auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
            object::pair{std::string("prompt"), finalPrompt}, 
            object::pair{std::string("system"), _rolePrompt}, 
            object::pair{std::string("temperature"), 0.2}, 
            object::pair{std::string("maxTokens"), 4096}, 
            object::pair{std::string("object"), true}
        }); });
        console->log(std::string("intel:buy-signal - response"), response);
        responseContent = as<std::shared_ptr<IBuySignalOutput>>(parseJSONObjectFromText(response));
        retries++;
        if (AND((AND((!responseContent->recommended_buy), (!responseContent->reason))), (!responseContent->recommend_buy_address))) {
            logger->warn(std::string("*** Missing required fields, retrying... generateSignal ***"));
        }
    }
    if (!responseContent->recommend_buy_address) {
        console->warn(std::string("buy-signal::generateSignal - no buy recommendation"));
        return false;
    }
    if (!responseContent->recommend_buy_address->match((new RegExp(std::string("^[1-9A-HJ-NP-Za-km-z]{32,44}"))))) {
        logger->error(std::string("Invalid Solana token address"), object{
            object::pair{std::string("address"), responseContent->recommend_buy_address}
        });
        return false;
    }
    auto apiKey = this->runtime->getSetting(std::string("BIRDEYE_API_KEY"));
    if (!apiKey) {
        logger->error(std::string("BIRDEYE_API_KEY not found in runtime settings"));
        return false;
    }
    auto BIRDEYE_API = std::string("https://public-api.birdeye.so");
    auto endpoint = string_empty + BIRDEYE_API + std::string("/defi/token_overview");
    auto url = string_empty + endpoint + std::string("?address=") + responseContent->recommend_buy_address + string_empty;
    logger->debug(std::string("Making Birdeye API request"), object{
        object::pair{std::string("url"), std::string("url")}, 
        object::pair{std::string("address"), responseContent->recommend_buy_address}
    });
    auto options = object{
        object::pair{std::string("method"), std::string("GET")}, 
        object::pair{std::string("headers"), object{
            object::pair{std::string("accept"), std::string("application/json")}, 
            object::pair{std::string("x-chain"), std::string("solana")}, 
            object::pair{std::string("X-API-KEY"), apiKey}
        }}
    };
    try
    {
        auto res = std::async([=]() { fetch(url, options); });
        if (!res->ok) {
            auto errorText = std::async([=]() { res->text(); });
            logger->error(std::string("Birdeye API request failed"), object{
                object::pair{std::string("status"), res->status}, 
                object::pair{std::string("statusText"), res->statusText}, 
                object::pair{std::string("error"), errorText}, 
                object::pair{std::string("address"), responseContent->recommend_buy_address}
            });
            throw any(std::make_shared<Error>(std::string("Birdeye marketcap request failed: ") + res->status + std::string(" ") + res->statusText + string_empty));
        }
        auto resJson = std::async([=]() { res->json(); });
        auto marketcap = resJson["data"]["marketCap"];
        if (!marketcap) {
            logger->warn(std::string("buy-signal: No marketcap data returned from Birdeye"), object{
                object::pair{std::string("response"), resJson}, 
                object::pair{std::string("address"), responseContent->recommend_buy_address}
            });
        }
        responseContent->marketcap = Number(OR((marketcap), (0)));
    }
    catch (const any& error)
    {
        logger->error(std::string("Error fetching marketcap data:"), error);
        responseContent->marketcap = 0;
    }
    this->runtime->emitEvent(std::string("SPARTAN_TRADE_BUY_SIGNAL"), responseContent);
    std::async([=]() { this->runtime->setCache<any>(std::string("buy_signals"), object{
        object::pair{std::string("key"), std::string("BUY_SIGNAL")}, 
        object::pair{std::string("data"), responseContent}
    }); });
    return true;
}

any BuySignal::getBalance()
{
    auto url = std::string("https://zondra-wil7oz-fast-mainnet.helius-rpc.com");
    auto headers = object{
        object::pair{std::string("Content-Type"), std::string("application/json")}
    };
    auto data = object{
        object::pair{std::string("jsonrpc"), std::string("2.0")}, 
        object::pair{std::string("id"), 1}, 
        object::pair{std::string("method"), std::string("getBalance")}, 
        object::pair{std::string("params"), array<string>{ DEGEN_WALLET }}
    };
    auto response = std::async([=]() { fetch(url, object{
        object::pair{std::string("method"), std::string("POST")}, 
        object::pair{std::string("headers"), headers}, 
        object::pair{std::string("body"), JSON->stringify(data)}
    }); });
    auto result = std::async([=]() { response->json(); });
    auto lamportsBalance = result["result"]["value"];
    return lamportsBalance / 1000000000;
}

string DEGEN_WALLET = std::string("BzsJQeZ7cvk3pTHmKeuvdhNDkDxcZ6uCXxW2rjwC7RTq");
string _rolePrompt = std::string("You are a buy signal analyzer.");
string _template = std::string("\
I want you to give a crypto buy signal based on both the sentiment analysis as well as the trending tokens.\
Only choose a token that occurs in both the Trending Tokens list as well as the Sentiment analysis. This ensures we have the proper token address.\
The sentiment score has a range of -100 to 100, with -100 indicating extreme negativity and 100 indicating extreme positiveness.\
My current balance is {{solana_balance}} SOL, If I have less than 0.3 SOL then I should not buy unless it's really good opportunity.\
Also let me know what a good amount would be to buy. Buy amount should at least be 0.05 SOL and maximum 0.25 SOL.\
\
Sentiment analysis:\
\
{{sentiment}}\
\
Trending tokens:\
\
{{trending_tokens}}\
\
Only return the following JSON:\
\
{\
recommended_buy: "the symbol of the token for example DEGENAI",\
recommend_buy_address: "the address of the token to purchase, for example: 2sCUCJdVkmyXp4dT8sFaA9LKgSMK4yDPi9zLHiwXpump",\
reason: "the reason why you think this is a good buy, and why you chose the specific amount",\
buy_amount: "number, for example: 0.1"\
}");

void Main(void)
{
}

MAIN
