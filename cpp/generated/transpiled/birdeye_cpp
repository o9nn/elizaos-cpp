#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenIntel/tasks/birdeye.h"

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

void Birdeye::fillTimeframe()
{
    auto memories = std::async([=]() { this->runtime->getMemories(object{
        object::pair{std::string("tableName"), std::string("messages")}, 
        object::pair{std::string("roomId"), this->sentimentRoomId}, 
        object::pair{std::string("end"), Date->now()}, 
        object::pair{std::string("count"), 1}
    }); });
    auto lastMemory = as<any>(const_(memories)[0]);
    auto lookUpDate = lastMemory["content"]["metadata"]["timeslot"];
    auto start = std::make_shared<Date>(OR((lookUpDate), (std::string("2025-01-01T00:00:00.000Z"))));
    start->setUTCHours(0, 0, 0, 0);
    auto today = std::make_shared<Date>();
    today->setUTCHours(23, 59, 59, 999);
    auto diff = Math->floor((today->getTime() - start->getTime()) / (1000 * 60 * 60 * 24));
    for (auto day = 0; day <= diff; day++)
    {
        auto now = std::make_shared<Date>(start);
        now->setUTCDate(start->getUTCDate() + day);
        for (auto hour = 0; hour <= 23; hour++)
        {
            auto timeslot = std::make_shared<Date>(now);
            timeslot->setUTCHours(hour, 0, 0, 0);
            auto rightNow = std::make_shared<Date>();
            if (timeslot > rightNow) {
                break;
            }
            std::async([=]() { this->runtime->createMemory(object{
                object::pair{std::string("id"), createUniqueUuid(this->runtime, std::string("sentiment-") + timeslot->toISOString() + string_empty)}, 
                object::pair{std::string("entityId"), this->runtime->agentId}, 
                object::pair{std::string("agentId"), this->runtime->agentId}, 
                object::pair{std::string("content"), object{
                    object::pair{std::string("text"), string_empty}, 
                    object::pair{std::string("source"), std::string("sentiment-analysis")}, 
                    object::pair{std::string("metadata"), object{
                        object::pair{std::string("timeslot"), timeslot->toISOString()}, 
                        object::pair{std::string("processed"), false}
                    }}
                }}, 
                object::pair{std::string("roomId"), this->sentimentRoomId}, 
                object::pair{std::string("createdAt"), timeslot->getTime()}
            }, std::string("messages")); });
        }
    }
    logger->info(std::string("Filled timeframe slots for sentiment analysis"));
}

any Birdeye::parseTweets()
{
    std::async([=]() { this->fillTimeframe(); });
    auto now = std::make_shared<Date>();
    auto oneHourAgo = std::make_shared<Date>(now->getTime() - 60 * 60 * 1000);
    auto twoDaysAgo = std::make_shared<Date>(now->getTime() - 2 * 24 * 60 * 60 * 1000);
    auto memories = std::async([=]() { this->runtime->getMemories(object{
        object::pair{std::string("tableName"), std::string("messages")}, 
        object::pair{std::string("roomId"), this->sentimentRoomId}, 
        object::pair{std::string("start"), twoDaysAgo->getTime()}, 
        object::pair{std::string("end"), oneHourAgo->getTime()}
    }); });
    auto sentiment = (as<array<any>>(memories))->find([=](auto m) mutable
    {
        return !m["content"]["metadata"]["processed"];
    }
    );
    if (!sentiment) {
        logger->debug(std::string("No unprocessed timeslots available."));
        return true;
    }
    logger->info(std::string("Trying to process ") + sentiment["content"]["metadata"]["timeslot"] + string_empty);
    auto timeslot = std::make_shared<Date>(sentiment["content"]["metadata"]["timeslot"]);
    auto fromDate = std::make_shared<Date>(timeslot->getTime() - 60 * 60 * 1000 + 1000);
    auto toDate = timeslot;
    auto tweets = std::async([=]() { this->runtime->getMemories(object{
        object::pair{std::string("tableName"), std::string("messages")}, 
        object::pair{std::string("roomId"), this->twitterFeedRoomId}, 
        object::pair{std::string("start"), fromDate->getTime()}, 
        object::pair{std::string("end"), toDate->getTime()}
    }); });
    if (OR((!tweets), (tweets->length == 0))) {
        logger->info(std::string("No tweets to process for timeslot ") + timeslot->toISOString() + string_empty);
        std::async([=]() { this->runtime->createMemory(object{
            object::pair{std::string("id"), sentiment["id"]}, 
            object::pair{std::string("entityId"), sentiment["entityId"]}, 
            object::pair{std::string("agentId"), sentiment["agentId"]}, 
            object::pair{std::string("content"), utils::assign(object{
                , 
                object::pair{std::string("metadata"), utils::assign(object{
                    , 
                    object::pair{std::string("processed"), true}
                }, sentiment["content"]["metadata"])}
            }, sentiment["content"])}, 
            object::pair{std::string("roomId"), sentiment["roomId"]}, 
            object::pair{std::string("createdAt"), sentiment["createdAt"]}
        }, std::string("messages")); });
        return true;
    }
    auto tweetArray = tweets->map([=](auto tweet) mutable
    {
        auto content = as<any>(tweet["content"]);
        return std::string("username: ") + (OR((content["tweet"]["username"]), (std::string("unknown")))) + std::string(" tweeted: ") + content["text"] + string_empty + (content["tweet"]["likes"]) ? any(std::string(" with ") + content["tweet"]["likes"] + std::string(" likes")) : any(string_empty) + string_empty + (content["tweet"]["retweets"]) ? any(std::string(" and ") + content["tweet"]["retweets"] + std::string(" retweets")) : any(string_empty) + std::string(".");
    }
    );
    auto bulletpointTweets = makeBulletpointList(tweetArray);
    auto prompt = template->replace(std::string("{{tweets}}"), bulletpointTweets);
    auto response = std::async([=]() { this->runtime->useModel(ModelType->TEXT_LARGE, object{
        object::pair{std::string("prompt"), std::string("prompt")}, 
        object::pair{std::string("system"), rolePrompt}, 
        object::pair{std::string("temperature"), 0.2}, 
        object::pair{std::string("maxTokens"), 4096}, 
        object::pair{std::string("object"), true}
    }); });
    auto json = JSON->parse(OR((response), (std::string("{}"))));
    std::async([=]() { this->runtime->createMemory(object{
        object::pair{std::string("id"), sentiment["id"]}, 
        object::pair{std::string("entityId"), sentiment["entityId"]}, 
        object::pair{std::string("agentId"), sentiment["agentId"]}, 
        object::pair{std::string("content"), object{
            object::pair{std::string("text"), json["text"]}, 
            object::pair{std::string("source"), std::string("sentiment-analysis")}, 
            object::pair{std::string("metadata"), utils::assign(object{
                , 
                object::pair{std::string("occuringTokens"), json["occuringTokens"]}, 
                object::pair{std::string("processed"), true}
            }, sentiment["content"]["metadata"])}
        }}, 
        object::pair{std::string("roomId"), sentiment["roomId"]}, 
        object::pair{std::string("createdAt"), sentiment["createdAt"]}
    }, std::string("messages")); });
    logger->info(std::string("Successfully processed timeslot ") + sentiment["content"]["metadata"]["timeslot"] + string_empty);
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
