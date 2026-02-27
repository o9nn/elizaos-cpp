#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/plugin-birdeye/src/tasks/birdeye.h"

string makeBulletpointList(array<string> array)
{
    return array->map([=](auto a) mutable
    {
        return std::string(" - ") + a + string_empty;
    }
    )->join(std::string("\
"));
};


Birdeye::Birdeye(std::shared_ptr<IAgentRuntime> runtime) {
    auto apiKey = runtime->getSetting(std::string("BIRDEYE_API_KEY"));
    if (!apiKey) {
        throw any(std::make_shared<Error>(std::string("Failed to initialize Birdeye provider due to missing API key.")));
    }
    this->apiKey = apiKey;
    this->sentimentRoomId = createUniqueUuid(runtime, std::string("sentiment-analysis"));
    this->twitterFeedRoomId = createUniqueUuid(runtime, std::string("twitter-feed"));
    this->runtime = runtime;
}

any Birdeye::syncWalletHistory()
{
    try
    {
        auto publicKey = OR((this->runtime->getSetting(std::string("SOLANA_PUBLIC_KEY"))), (std::string("BzsJQeZ7cvk3pTHmKeuvdhNDkDxcZ6uCXxW2rjwC7RTq")));
        auto options = object{
            object::pair{std::string("method"), std::string("GET")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("accept"), std::string("application/json")}, 
                object::pair{std::string("x-chain"), std::string("solana")}, 
                object::pair{std::string("X-API-KEY"), this->apiKey}
            }}
        };
        auto res = std::async([=]() { fetch(std::string("https://public-api.birdeye.so/v1/wallet/tx_list?wallet=") + publicKey + std::string("&limit=100"), options); });
        auto resp = std::async([=]() { res->json(); });
        auto birdeyeData = OR((resp["data"]["solana"]), (array<any>()));
        auto transactions = birdeyeData["map"]([=](auto tx) mutable
        {
            return (object{
                object::pair{std::string("txHash"), tx["txHash"]}, 
                object::pair{std::string("blockTime"), std::make_shared<Date>(tx["blockTime"])}, 
                object::pair{std::string("data"), tx}
            });
        }
        );
        try
        {
            auto cachedTxs = std::async([=]() { this->runtime->getCache<array<std::shared_ptr<TransactionHistory>>>(std::string("transaction_history")); });
            if (AND((cachedTxs), (Array->isArray(cachedTxs)))) {
                for (auto& cachedTx : cachedTxs)
                {
                    if (!transactions->some([=](auto tx) mutable
                    {
                        return tx->txHash == cachedTx["txHash"];
                    }
                    )) {
                        transactions->push(cachedTx);
                    }
                }
            }
        }
        catch (const any& error)
        {
            logger->debug(std::string("Failed to get cached transactions, continuing with Birdeye data only"));
        }
        for (auto& tx : transactions)
        {
            if (type_of(tx->blockTime) == std::string("string")) {
                tx->blockTime = std::make_shared<Date>(tx->blockTime);
            }
        }
        transactions->sort([=](auto a, auto b) mutable
        {
            return b->blockTime->getTime() - a->blockTime->getTime();
        }
        );
        try
        {
            std::async([=]() { this->runtime->setCache<array<std::shared_ptr<TransactionHistory>>>(std::string("transaction_history"), transactions); });
            logger->debug(std::string("Updated transaction history with ") + transactions->get_length() + std::string(" transactions"));
        }
        catch (const any& error)
        {
            logger->debug(std::string("Failed to set transaction cache, continuing without caching"), error);
        }
        return transactions;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to sync wallet history from Birdeye"), error);
        return array<any>();
    }
}

void Birdeye::syncWalletPortfolio()
{
    auto options = object{
        object::pair{std::string("method"), std::string("GET")}, 
        object::pair{std::string("headers"), object{
            object::pair{std::string("accept"), std::string("application/json")}, 
            object::pair{std::string("x-chain"), std::string("solana")}, 
            object::pair{std::string("X-API-KEY"), this->apiKey}
        }}
    };
    auto publicKey = OR((this->runtime->getSetting(std::string("SOLANA_PUBLIC_KEY"))), (std::string("BzsJQeZ7cvk3pTHmKeuvdhNDkDxcZ6uCXxW2rjwC7RTq")));
    auto res = std::async([=]() { fetch(std::string("https://public-api.birdeye.so/v1/wallet/token_list?wallet=") + publicKey + string_empty, options); });
    auto resp = std::async([=]() { res->json(); });
    auto data = resp["data"];
    std::async([=]() { this->runtime->setCache<std::shared_ptr<Portfolio>>(std::string("portfolio"), object{
        object::pair{std::string("key"), std::string("PORTFOLIO")}, 
        object::pair{std::string("data"), std::string("data")}
    }); });
}

any Birdeye::syncWallet()
{
    std::async([=]() { this->syncWalletHistory(); });
    std::async([=]() { this->syncWalletPortfolio(); });
    return true;
}

string rolePrompt = std::string("You are a sentiment analyzer for cryptocurrency and market data.");
string template = std::string("Write a summary of what is happening in the tweets. The main topic is the cryptocurrency market.\
You will also be analyzing the tokens that occur in the tweet and tell us whether their sentiment is positive or negative.\
\
## Analyze the followings tweets:\
{{tweets}}\
\
Strictly return the following json:\
\
{\
   "text":"the summary of what has happened in those tweets, with a max length of 200 characters",\
   "occuringTokens":[\
      {\
         "token":"the token symbol, like: ETH, SOL, BTC etc.",\
         "sentiment":"positive is between 1 and 100 and negative is from -1 to -100",\
         "reason":"a short sentence explaining the reason for this sentiment score"\
      }\
   ]\
}");

void Main(void)
{
}

MAIN
