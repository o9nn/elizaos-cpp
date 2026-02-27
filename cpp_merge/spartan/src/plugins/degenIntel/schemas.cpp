#include "schemas.hpp"

any TokenSchema = z->object(object{
    object::pair{std:("provider"), z->string()}, 
    object::pair{std:("rank"), z->number()}, 
    object::pair{std:("__v"), z->number()}, 
    object::pair{std:("address"), z->string()}, 
    object::pair{std:("chain"), z->string()}, 
    object::pair{std:("createdAt"), z->string()->datetime()}, 
    object::pair{std:("decimals"), z->number()}, 
    object::pair{std:("last_updated"), z->string()->datetime()}, 
    object::pair{std:("liquidity"), z->number()}, 
    object::pair{std:("logoURI"), z->string()->url()}, 
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("price"), z->number()}, 
    object::pair{std:("price24hChangePercent"), z->number()}, 
    object::pair{std:("symbol"), z->string()}, 
    object::pair{std:("updatedAt"), z->string()->datetime()}, 
    object::pair{std:("volume24hUSD"), z->number()}, 
    object::pair{std:("marketcap"), z->number()}
});
any TokenArraySchema = z->array(TokenSchema);
any TokenRequestSchema = z->object(object{
    object::pair{std:("address"), z->string()->min(1, std:("Address is required"))}
});
any TweetSchema = z->object(object{
    object::pair{std:("_id"), z->string()}, 
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("__v"), z->number()}, 
    object::pair{std:("createdAt"), z->string()->datetime()}, 
    object::pair{std:("likes"), z->number()}, 
    object::pair{std:("retweets"), z->number()}, 
    object::pair{std:("text"), z->string()}, 
    object::pair{std:("timestamp"), z->string()->datetime()}, 
    object::pair{std:("updatedAt"), z->string()->datetime()}, 
    object::pair{std:("username"), z->string()}
});
any TweetArraySchema = z->array(TweetSchema);
any SentimentSchema = z->object(object{
    object::pair{std:("timeslot"), z->string()->datetime()}, 
    object::pair{std:("createdAt"), z->string()->datetime()}, 
    object::pair{std:("occuringTokens"), z->array(z->object(object{
        object::pair{std:("token"), z->string()}, 
        object::pair{std:("sentiment"), z->number()}, 
        object::pair{std:("reason"), z->string()}
    }))}, 
    object::pair{std:("processed"), z->boolean()}, 
    object::pair{std:("updatedAt"), z->string()->datetime()}, 
    object::pair{std:("text"), z->string()}
});
any SentimentArraySchema = z->array(SentimentSchema);
any WalletSchema = z->object(object{
    object::pair{std:("wallet"), z->string()}, 
    object::pair{std:("totalUsd"), z->number()}, 
    object::pair{std:("items"), z->array(z->object(object{
        object::pair{std:("address"), z->string()}, 
        object::pair{std:("decimals"), z->number()}, 
        object::pair{std:("balance"), z->number()}, 
        object::pair{std:("uiAmount"), z->number()}, 
        object::pair{std:("chainId"), z->string()}, 
        object::pair{std:("name"), z->string()}, 
        object::pair{std:("symbol"), z->string()}, 
        object::pair{std:("icon"), z->string()->url()}, 
        object::pair{std:("logoURI"), z->string()->url()}, 
        object::pair{std:("priceUsd"), z->number()}, 
        object::pair{std:("valueUsd"), z->number()}
    }))}
});
any BuySignalSchema = z->object(object{
    object::pair{std:("recommended_buy"), z->string()}, 
    object::pair{std:("recommended_buy_address"), z->string()}, 
    object::pair{std:("reason"), z->string()}, 
    object::pair{std:("marketcap"), z->number()}, 
    object::pair{std:("buy_amount"), z->string()}
});
any StatisticsSchema = z->object(object{
    object::pair{std:("tweets"), z->number()}, 
    object::pair{std:("sentiment"), z->number()}, 
    object::pair{std:("tokens"), z->number()}
});

void Main(void)
{
}

MAIN
