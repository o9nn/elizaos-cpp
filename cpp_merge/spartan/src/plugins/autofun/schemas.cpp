#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofun/schemas.h"

any TokenSchema = z->object(object{
    object::pair{std::string("provider"), z->string()}, 
    object::pair{std::string("rank"), z->number()}, 
    object::pair{std::string("__v"), z->number()}, 
    object::pair{std::string("address"), z->string()}, 
    object::pair{std::string("chain"), z->string()}, 
    object::pair{std::string("createdAt"), z->string()->datetime()}, 
    object::pair{std::string("decimals"), z->number()}, 
    object::pair{std::string("last_updated"), z->string()->datetime()}, 
    object::pair{std::string("liquidity"), z->number()}, 
    object::pair{std::string("logoURI"), z->string()->url()}, 
    object::pair{std::string("name"), z->string()}, 
    object::pair{std::string("price"), z->number()}, 
    object::pair{std::string("price24hChangePercent"), z->number()}, 
    object::pair{std::string("symbol"), z->string()}, 
    object::pair{std::string("updatedAt"), z->string()->datetime()}, 
    object::pair{std::string("volume24hUSD"), z->number()}, 
    object::pair{std::string("marketcap"), z->number()}
});
any TokenArraySchema = z->array(TokenSchema);
any TokenRequestSchema = z->object(object{
    object::pair{std::string("address"), z->string()->min(1, std::string("Address is required"))}
});
any TweetSchema = z->object(object{
    object::pair{std::string("_id"), z->string()}, 
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("__v"), z->number()}, 
    object::pair{std::string("createdAt"), z->string()->datetime()}, 
    object::pair{std::string("likes"), z->number()}, 
    object::pair{std::string("retweets"), z->number()}, 
    object::pair{std::string("text"), z->string()}, 
    object::pair{std::string("timestamp"), z->string()->datetime()}, 
    object::pair{std::string("updatedAt"), z->string()->datetime()}, 
    object::pair{std::string("username"), z->string()}
});
any TweetArraySchema = z->array(TweetSchema);
any SentimentSchema = z->object(object{
    object::pair{std::string("timeslot"), z->string()->datetime()}, 
    object::pair{std::string("createdAt"), z->string()->datetime()}, 
    object::pair{std::string("occuringTokens"), z->array(z->object(object{
        object::pair{std::string("token"), z->string()}, 
        object::pair{std::string("sentiment"), z->number()}, 
        object::pair{std::string("reason"), z->string()}
    }))}, 
    object::pair{std::string("processed"), z->boolean()}, 
    object::pair{std::string("updatedAt"), z->string()->datetime()}, 
    object::pair{std::string("text"), z->string()}
});
any SentimentArraySchema = z->array(SentimentSchema);
any WalletSchema = z->object(object{
    object::pair{std::string("wallet"), z->string()}, 
    object::pair{std::string("totalUsd"), z->number()}, 
    object::pair{std::string("items"), z->array(z->object(object{
        object::pair{std::string("address"), z->string()}, 
        object::pair{std::string("decimals"), z->number()}, 
        object::pair{std::string("balance"), z->number()}, 
        object::pair{std::string("uiAmount"), z->number()}, 
        object::pair{std::string("chainId"), z->string()}, 
        object::pair{std::string("name"), z->string()}, 
        object::pair{std::string("symbol"), z->string()}, 
        object::pair{std::string("icon"), z->string()->url()}, 
        object::pair{std::string("logoURI"), z->string()->url()}, 
        object::pair{std::string("priceUsd"), z->number()}, 
        object::pair{std::string("valueUsd"), z->number()}
    }))}
});
any BuySignalSchema = z->object(object{
    object::pair{std::string("recommended_buy"), z->string()}, 
    object::pair{std::string("recommended_buy_address"), z->string()}, 
    object::pair{std::string("reason"), z->string()}, 
    object::pair{std::string("marketcap"), z->number()}, 
    object::pair{std::string("buy_amount"), z->string()}
});
any StatisticsSchema = z->object(object{
    object::pair{std::string("tweets"), z->number()}, 
    object::pair{std::string("sentiment"), z->number()}, 
    object::pair{std::string("tokens"), z->number()}
});

void Main(void)
{
}

MAIN
