#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_BIRDEYE_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SPARTAN_SRC_PLUGINS_DEGENINTEL_TASKS_BIRDEYE_H
#include "core.h"
#include "@elizaos/core.h"
#include "../types.h"

class TransactionHistory;
class Portfolio;
class SentimentContent;
class Birdeye;

class TransactionHistory : public object, public std::enable_shared_from_this<TransactionHistory> {
public:
    using std::enable_shared_from_this<TransactionHistory>::shared_from_this;
    string txHash;

    std::shared_ptr<Date> blockTime;

    any data;
};

class Portfolio : public object, public std::enable_shared_from_this<Portfolio> {
public:
    using std::enable_shared_from_this<Portfolio>::shared_from_this;
    string key;

    any data;
};

class SentimentContent : public Content, public std::enable_shared_from_this<SentimentContent> {
public:
    using std::enable_shared_from_this<SentimentContent>::shared_from_this;
    string text;

    string source;

    object metadata;
};

extern string rolePrompt;
extern string template;
string makeBulletpointList(array<string> array);

class Birdeye : public object, public std::enable_shared_from_this<Birdeye> {
public:
    using std::enable_shared_from_this<Birdeye>::shared_from_this;
    string apiKey;

    std::shared_ptr<UUID> sentimentRoomId;

    std::shared_ptr<UUID> twitterFeedRoomId;

    std::shared_ptr<IAgentRuntime> runtime;

    Birdeye(std::shared_ptr<IAgentRuntime> runtime);
    virtual any syncWalletHistory();
    virtual void syncWalletPortfolio();
    virtual any syncWallet();
    template <typename P0>
    std::shared_ptr<Promise<boolean>> syncTrendingTokens(P0 chain);
    virtual void fillTimeframe();
    virtual any parseTweets();
};

using _default = Birdeye;
template <typename P0>
std::shared_ptr<Promise<boolean>> Birdeye::syncTrendingTokens(P0 chain)
{
    try
    {
        auto options = object{
            object::pair{std::string("method"), std::string("GET")}, 
            object::pair{std::string("headers"), object{
                object::pair{std::string("accept"), std::string("application/json")}, 
                object::pair{std::string("x-chain"), chain}, 
                object::pair{std::string("X-API-KEY"), this->apiKey}
            }}
        };
        auto cachedTokens = std::async([=]() { this->runtime->getCache<array<std::shared_ptr<IToken>>>(std::string("tokens_") + chain + string_empty); });
        auto tokens = (cachedTokens) ? any(cachedTokens) : any(array<any>());
        for (auto batch = 0; batch < 5; batch++)
        {
            auto currentOffset = batch * 20;
            auto res = std::async([=]() { fetch(std::string("https://public-api.birdeye.so/defi/token_trending?sort_by=rank&sort_type=asc&offset=") + currentOffset + std::string("&limit=20"), options); });
            auto resp = std::async([=]() { res->json(); });
            auto data = resp["data"];
            auto last_updated = std::make_shared<Date>(data["updateUnixTime"] * 1000);
            auto newTokens = data["tokens"];
            if (!newTokens) {
                continue;
            }
            for (auto& token : newTokens)
            {
                auto existingIndex = tokens->findIndex([=](auto t) mutable
                {
                    return AND((AND((t->provider == std::string("birdeye")), (t->rank == token["rank"]))), (t->chain == chain));
                }
                );
                auto tokenData = object{
                    object::pair{std::string("address"), token["address"]}, 
                    object::pair{std::string("chain"), chain}, 
                    object::pair{std::string("provider"), std::string("birdeye")}, 
                    object::pair{std::string("decimals"), OR((token["decimals"]), (0))}, 
                    object::pair{std::string("liquidity"), OR((token["liquidity"]), (0))}, 
                    object::pair{std::string("logoURI"), OR((token["logoURI"]), (string_empty))}, 
                    object::pair{std::string("name"), OR((token["name"]), (token["symbol"]))}, 
                    object::pair{std::string("symbol"), token["symbol"]}, 
                    object::pair{std::string("marketcap"), 0}, 
                    object::pair{std::string("volume24hUSD"), OR((token["volume24hUSD"]), (0))}, 
                    object::pair{std::string("rank"), OR((token["rank"]), (0))}, 
                    object::pair{std::string("price"), OR((token["price"]), (0))}, 
                    object::pair{std::string("price24hChangePercent"), OR((token["price24hChangePercent"]), (0))}, 
                    object::pair{std::string("last_updated"), std::string("last_updated")}
                };
                if (existingIndex >= 0) {
                    tokens[existingIndex] = tokenData;
                } else {
                    tokens->push(tokenData);
                }
            }
            std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
            {
                return setTimeout(resolve, 250);
            }
            ); });
        }
        std::async([=]() { this->runtime->setCache<array<std::shared_ptr<IToken>>>(std::string("tokens_") + chain + string_empty, tokens); });
        logger->debug(std::string("Updated ") + chain + std::string(" tokens cache with ") + tokens->get_length() + std::string(" tokens"));
        return true;
    }
    catch (const any& error)
    {
        logger->error(std::string("Failed to sync trending tokens"), error);
        throw any(error);
    }
}

#endif
