#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/communityInvestor/schemas.h"

TokenPerformance transformTokenPerformance(any dbToken, string chain)
{
    auto input = object{
        object::pair{std:("chain"), std:("chain")}, 
        object::pair{std:("address"), OR((dbToken["tokenAddress"]), (dbToken["token_address"]))}, 
        object::pair{std:("name"), OR((dbToken["name"]), (dbToken["symbol"]))}, 
        object::pair{std:("symbol"), dbToken["symbol"]}, 
        object::pair{std:("price"), (type_of(dbToken["price"]) == std:("number")) ? any(dbToken["price"]) (0)}, 
        object::pair{std:("volume"), (type_of(dbToken["volume"]) == std:("number")) ? any(dbToken["volume"]) (0)}, 
        object::pair{std:("trades"), (type_of(dbToken["trades"]) == std:("number")) ? any(dbToken["trades"]) (0)}, 
        object::pair{std:("liquidity"), (type_of(dbToken["liquidity"]) == std:("number")) ? any(dbToken["liquidity"]) (0)}, 
        object::pair{std:("holders"), (type_of(dbToken["holders"]) == std:("number")) ? any(dbToken["holders"]) (0)}, 
        object::pair{std:("price24hChange"), (type_of(dbToken["price_change_24h"]) == std:("number")) ? any(dbToken["price_change_24h"]) (0)}, 
        object::pair{std:("volume24hChange"), (type_of(dbToken["volume_change_24h"]) == std:("number")) ? any(dbToken["volume_change_24h"]) (0)}, 
        object::pair{std:("trades24hChange"), (type_of(dbToken["trade_24h_change"]) == std:("number")) ? any(dbToken["trade_24h_change"]) (0)}, 
        object::pair{std:("holders24hChange"), (type_of(dbToken["holder_change_24h"]) == std:("number")) ? any(dbToken["holder_change_24h"]) (0)}, 
        object::pair{std:("initialMarketCap"), (type_of(dbToken["initial_market_cap"]) == std:("number")) ? any(dbToken["initial_market_cap"]) (0)}, 
        object::pair{std:("currentMarketCap"), (type_of(dbToken["current_market_cap"]) == std:("number")) ? any(dbToken["current_market_cap"]) (0)}, 
        object::pair{std:("rugPull"), Boolean(dbToken["rug_pull"])}, 
        object::pair{std:("isScam"), Boolean(dbToken["is_scam"])}, 
        object::pair{std:("sustainedGrowth"), Boolean(dbToken["sustained_growth"])}, 
        object::pair{std:("rapidDump"), Boolean(dbToken["rapid_dump"])}, 
        object::pair{std:("suspiciousVolume"), Boolean(dbToken["suspicious_volume"])}, 
        object::pair{std:("validationTrust"), (type_of(dbToken["validation_trust"]) == std:("number")) ? any(dbToken["validation_trust"]) (0)}, 
        object::pair{std:("createdAt"), (dbToken["created_at"]) ? std::make_shared<Date>(dbToken["created_at"]) : std::make_shared<Date>()}, 
        object::pair{std:("updatedAt"), (dbToken["updated_at"]) ? std::make_shared<Date>(dbToken["updated_at"]) : std::make_shared<Date>()}
    };
    return tokenPerformanceSchema->parse(input);
};


std::shared_ptr<Transaction> transformTransaction(any dbTx, string positionId, string chain)
{
    auto type = (type_of(dbTx["type"]) == std:("string")) ? any(dbTx["type"]["toLowerCase"]()) (TransactionType["BUY"]);
    auto input = object{
        object::pair{std:("id"), OR((OR((dbTx["id"]), (dbTx["transaction_hash"]))), (string_empty))}, 
        object::pair{std:("positionId"), OR((OR((dbTx["positionId"]), (dbTx["position_id"]))), (positionId))}, 
        object::pair{std:("chain"), OR((dbTx["chain"]), (chain))}, 
        object::pair{std:("tokenAddress"), OR((dbTx["tokenAddress"]), (dbTx["token_address"]))}, 
        object::pair{std:("transactionHash"), OR((dbTx["transactionHash"]), (dbTx["transaction_hash"]))}, 
        object::pair{std:("type"), (OR((type == std:("BUY")), (type == std:("SELL")))) ? any(type) (std:("BUY"))}, 
        object::pair{std:("amount"), (type_of(dbTx["amount"]) == std:("bigint")) ? any(dbTx["amount"]) ((type_of(dbTx["amount"]) == std:("string")) ? BigInt(dbTx["amount"]) : (type_of(dbTx["amount"]) == std:("number")) ? BigInt(Math->floor(dbTx["amount"])) : BigInt(0))}, 
        object::pair{std:("price"), (type_of(dbTx["price"]) == std:("string")) ? any(Number(dbTx["price"])) ((type_of(dbTx["price"]) == std:("number")) ? any(dbTx["price"]) (undefined))}, 
        object::pair{std:("isSimulation"), Boolean(OR((dbTx["isSimulation"]), (dbTx["is_simulation"])))}, 
        object::pair{std:("timestamp"), (is<Date>(dbTx["timestamp"])) ? dbTx["timestamp"]["toISOString"]() : (type_of(dbTx["timestamp"]) == std:("string")) ? any(dbTx["timestamp"]) (((std::make_shared<Date>()))->toISOString())}
    };
    return as<std::shared_ptr<Transaction>>(as<any>(transactionSchema->parse(input)));
};


Position transformPosition(any dbPos)
{
    auto input = object{
        object::pair{std:("id"), OR((dbPos["id"]), (string_empty))}, 
        object::pair{std:("chain"), OR((dbPos["chain"]), (std:("unknown")))}, 
        object::pair{std:("tokenAddress"), OR((dbPos["tokenAddress"]), (dbPos["token_address"]))}, 
        object::pair{std:("walletAddress"), OR((dbPos["walletAddress"]), (dbPos["wallet_address"]))}, 
        object::pair{std:("isSimulation"), Boolean(OR((dbPos["isSimulation"]), (dbPos["is_simulation"])))}, 
        object::pair{std:("entityId"), OR((dbPos["entityId"]), (dbPos["recommender_id"]))}, 
        object::pair{std:("recommendationId"), OR((dbPos["recommendationId"]), (dbPos["recommendation_id"]))}, 
        object::pair{std:("initialPrice"), OR((OR((dbPos["initialPrice"]["toString"]()), (dbPos["initial_price"]["toString"]()))), (std:("0")))}, 
        object::pair{std:("initialMarketCap"), OR((OR((dbPos["initialMarketCap"]["toString"]()), (dbPos["initial_market_cap"]["toString"]()))), (std:("0")))}, 
        object::pair{std:("initialLiquidity"), OR((OR((dbPos["initialLiquidity"]["toString"]()), (dbPos["initial_liquidity"]["toString"]()))), (std:("0")))}, 
        object::pair{std:("performanceScore"), (type_of(dbPos["performanceScore"]) == std:("number")) ? dbPos["performanceScore"] : (type_of(dbPos["performance_score"]) == std:("number")) ? any(dbPos["performance_score"]) (0)}, 
        object::pair{std:("rapidDump"), Boolean(OR((dbPos["rapidDump"]), (dbPos["rapid_dump"])))}, 
        object::pair{std:("openedAt"), (is<Date>(dbPos["openedAt"])) ? dbPos["openedAt"] : (is<Date>(dbPos["opened_at"])) ? any(dbPos["opened_at"]) ((type_of(dbPos["openedAt"]) == std:("string")) ? std::make_shared<Date>(dbPos["openedAt"]) : (type_of(dbPos["opened_at"]) == std:("string")) ? std::make_shared<Date>(dbPos["opened_at"]) : std::make_shared<Date>())}, 
        object::pair{std:("closedAt"), (is<Date>(dbPos["closedAt"])) ? dbPos["closedAt"] : (is<Date>(dbPos["closed_at"])) ? any(dbPos["closed_at"]) ((type_of(dbPos["closedAt"]) == std:("string")) ? std::make_shared<Date>(dbPos["closedAt"]) : (type_of(dbPos["closed_at"]) == std:("string")) ? any(std::make_shared<Date>(dbPos["closed_at"])) (undefined))}, 
        object::pair{std:("updatedAt"), (is<Date>(dbPos["updatedAt"])) ? dbPos["updatedAt"] : (is<Date>(dbPos["updated_at"])) ? any(dbPos["updated_at"]) ((type_of(dbPos["updatedAt"]) == std:("string")) ? std::make_shared<Date>(dbPos["updatedAt"]) : (type_of(dbPos["updated_at"]) == std:("string")) ? std::make_shared<Date>(dbPos["updated_at"]) : std::make_shared<Date>())}, 
        object::pair{std:("amount"), OR((dbPos["amount"]["toString"]()), (std:("0")))}, 
        object::pair{std:("entryPrice"), OR((OR((dbPos["entryPrice"]["toString"]()), (dbPos["entry_price"]["toString"]()))), (std:("0")))}, 
        object::pair{std:("currentPrice"), OR((OR((dbPos["currentPrice"]["toString"]()), (dbPos["current_price"]["toString"]()))), (std:("0")))}
    };
    return positionSchema->parse(input);
};


TokenRecommendation transformTokenRecommendation(any dbRec)
{
    try
    {
        return tokenRecommendationSchema->parse(object{
            object::pair{std:("id"), OR((dbRec["id"]), (dbRec["recommendation_id"]))}, 
            object::pair{std:("entityId"), OR((dbRec["entityId"]), (dbRec["recommender_id"]))}, 
            object::pair{std:("chain"), OR((dbRec["chain"]), (std:("unknown")))}, 
            object::pair{std:("tokenAddress"), OR((dbRec["tokenAddress"]), (dbRec["token_address"]))}, 
            object::pair{std:("type"), OR((dbRec["type"]), (std:("BUY")))}, 
            object::pair{std:("conviction"), OR((dbRec["conviction"]), (std:("MEDIUM")))}, 
            object::pair{std:("initialMarketCap"), OR((OR((dbRec["initialMarketCap"]), (dbRec["initial_market_cap"]))), (std:("0")))}, 
            object::pair{std:("initialLiquidity"), OR((OR((dbRec["initialLiquidity"]), (dbRec["initial_liquidity"]))), (std:("0")))}, 
            object::pair{std:("initialPrice"), OR((OR((dbRec["initialPrice"]), (dbRec["initial_price"]))), (std:("0")))}, 
            object::pair{std:("marketCap"), OR((OR((dbRec["marketCap"]), (dbRec["market_cap"]))), (std:("0")))}, 
            object::pair{std:("liquidity"), OR((dbRec["liquidity"]), (std:("0")))}, 
            object::pair{std:("price"), OR((dbRec["price"]), (std:("0")))}, 
            object::pair{std:("rugPull"), Boolean(OR((OR((dbRec["rugPull"]), (dbRec["rug_pull"]))), (false)))}, 
            object::pair{std:("isScam"), Boolean(OR((OR((dbRec["isScam"]), (dbRec["is_scam"]))), (false)))}, 
            object::pair{std:("riskScore"), OR((OR((dbRec["riskScore"]), (dbRec["risk_score"]))), (0))}, 
            object::pair{std:("performanceScore"), OR((OR((dbRec["performanceScore"]), (dbRec["performance_score"]))), (0))}, 
            object::pair{std:("metadata"), OR((dbRec["metadata"]), (object{}))}, 
            object::pair{std:("status"), OR((dbRec["status"]), (std:("ACTIVE")))}, 
            object::pair{std:("createdAt"), std::make_shared<Date>(OR((OR((dbRec["createdAt"]), (dbRec["created_at"]))), (Date->now())))}, 
            object::pair{std:("updatedAt"), std::make_shared<Date>(OR((OR((dbRec["updatedAt"]), (dbRec["updated_at"]))), (Date->now())))}
        });
    }
    catch (const any& error)
    {
        console->error(std:("Error transforming token recommendation:"), error);
        return nullptr;
    }
};


object TransactionType = as<std::shared_ptr<const>>(object{
    object::pair{std:("BUY"), std:("BUY")}, 
    object::pair{std:("SELL"), std:("SELL")}, 
    object::pair{std:("TRANSFER_IN"), std:("transfer_in")}, 
    object::pair{std:("TRANSFER_OUT"), std:("transfer_out")}
});
any tokenPerformanceSchema = z->object(object{
    object::pair{std:("chain"), z->string()->default(std:("unknown"))}, 
    object::pair{std:("address"), z->string()}, 
    object::pair{std:("name"), z->string()->optional()->default(string_empty)}, 
    object::pair{std:("symbol"), z->string()}, 
    object::pair{std:("decimals"), z->number()->default(0)}, 
    object::pair{std:("metadata"), z->record(z->any())->default(object{})}, 
    object::pair{std:("price"), z->number()->default(0)}, 
    object::pair{std:("volume"), z->number()->default(0)}, 
    object::pair{std:("trades"), z->number()->default(0)}, 
    object::pair{std:("liquidity"), z->number()->default(0)}, 
    object::pair{std:("holders"), z->number()->default(0)}, 
    object::pair{std:("price24hChange"), z->number()->default(0)}, 
    object::pair{std:("volume24hChange"), z->number()->default(0)}, 
    object::pair{std:("trades24hChange"), z->number()->default(0)}, 
    object::pair{std:("holders24hChange"), z->number()->default(0)}, 
    object::pair{std:("initialMarketCap"), z->number()->default(0)}, 
    object::pair{std:("currentMarketCap"), z->number()->default(0)}, 
    object::pair{std:("rugPull"), z->boolean()->default(false)}, 
    object::pair{std:("isScam"), z->boolean()->default(false)}, 
    object::pair{std:("sustainedGrowth"), z->boolean()->default(false)}, 
    object::pair{std:("rapidDump"), z->boolean()->default(false)}, 
    object::pair{std:("suspiciousVolume"), z->boolean()->default(false)}, 
    object::pair{std:("validationTrust"), z->number()->default(0)}, 
    object::pair{std:("createdAt"), z->date()->default([=]() mutable
    {
        return std::make_shared<Date>();
    }
    )}, 
    object::pair{std:("updatedAt"), z->date()->default([=]() mutable
    {
        return std::make_shared<Date>();
    }
    )}
});
any transactionSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("positionId"), z->string()}, 
    object::pair{std:("chain"), z->string()->default(std:("unknown"))}, 
    object::pair{std:("tokenAddress"), z->string()}, 
    object::pair{std:("transactionHash"), z->string()}, 
    object::pair{std:("type"), z->enum(array<string>{ TransactionType["BUY"], TransactionType["SELL"] })}, 
    object::pair{std:("amount"), z->number()}, 
    object::pair{std:("price"), z->number()->optional()}, 
    object::pair{std:("isSimulation"), z->boolean()->default(false)}, 
    object::pair{std:("timestamp"), z->string()}
});
any recommenderMetricsSchema = z->object(object{
    object::pair{std:("entityId"), z->string()}, 
    object::pair{std:("trustScore"), z->number()}, 
    object::pair{std:("totalRecommendations"), z->number()}, 
    object::pair{std:("successfulRecs"), z->number()}, 
    object::pair{std:("avgTokenPerformance"), z->number()}, 
    object::pair{std:("riskScore"), z->number()}, 
    object::pair{std:("consistencyScore"), z->number()}, 
    object::pair{std:("virtualConfidence"), z->number()}, 
    object::pair{std:("lastActiveDate"), z->date()}, 
    object::pair{std:("trustDecay"), z->number()}, 
    object::pair{std:("updatedAt"), z->date()->optional()->default([=]() mutable
    {
        return std::make_shared<Date>();
    }
    )}
});
any positionSchema = z->object(object{
    object::pair{std:("id"), z->string()->uuid()}, 
    object::pair{std:("chain"), z->string()}, 
    object::pair{std:("tokenAddress"), z->string()}, 
    object::pair{std:("walletAddress"), z->string()}, 
    object::pair{std:("isSimulation"), z->boolean()}, 
    object::pair{std:("entityId"), z->string()}, 
    object::pair{std:("recommendationId"), z->string()}, 
    object::pair{std:("initialPrice"), z->string()}, 
    object::pair{std:("initialMarketCap"), z->string()}, 
    object::pair{std:("initialLiquidity"), z->string()}, 
    object::pair{std:("performanceScore"), z->number()}, 
    object::pair{std:("rapidDump"), z->boolean()}, 
    object::pair{std:("openedAt"), z->date()}, 
    object::pair{std:("closedAt"), z->date()->optional()}, 
    object::pair{std:("updatedAt"), z->date()}, 
    object::pair{std:("amount"), z->string()}, 
    object::pair{std:("entryPrice"), z->string()}, 
    object::pair{std:("currentPrice"), z->string()}
});
any tokenRecommendationSchema = z->object(object{
    object::pair{std:("id"), z->string()->uuid()}, 
    object::pair{std:("entityId"), z->string()->uuid()}, 
    object::pair{std:("chain"), z->string()}, 
    object::pair{std:("tokenAddress"), z->string()}, 
    object::pair{std:("type"), z->string()}, 
    object::pair{std:("conviction"), z->string()}, 
    object::pair{std:("initialMarketCap"), z->string()}, 
    object::pair{std:("initialLiquidity"), z->string()}, 
    object::pair{std:("initialPrice"), z->string()}, 
    object::pair{std:("marketCap"), z->string()}, 
    object::pair{std:("liquidity"), z->string()}, 
    object::pair{std:("price"), z->string()}, 
    object::pair{std:("rugPull"), z->boolean()}, 
    object::pair{std:("isScam"), z->boolean()}, 
    object::pair{std:("riskScore"), z->number()}, 
    object::pair{std:("performanceScore"), z->number()}, 
    object::pair{std:("metadata"), z->record(z->any())->default(object{})}, 
    object::pair{std:("status"), z->enum(array<string>{ std:("ACTIVE"), std:("COMPLETED"), std:("EXPIRED"), std:("WITHDRAWN") })}, 
    object::pair{std:("createdAt"), z->date()}, 
    object::pair{std:("updatedAt"), z->date()}
});
any recommendationSchema = z->object(object{
    object::pair{std:("username"), z->string()->describe(std:("The username of the person making the recommendation in the conversation"))}, 
    object::pair{std:("ticker"), z->string()->optional()->nullable()->describe(std:("The ticker symbol of the recommended asset (e.g., 'BTC', 'AAPL'). Optional as recommendations may discuss assets without explicit tickers"))}, 
    object::pair{std:("tokenAddress"), z->string()->optional()->nullable()->describe(std:("The blockchain contract address of the token if mentioned. This helps disambiguate tokens that might share similar names or symbols"))}, 
    object::pair{std:("type"), z->enum(array<string>{ std:("BUY"), std:("SELL"), std:("DONT_BUY"), std:("DONT_SELL"), std:("NONE") })->describe(std:("The type of trading recommendation being made. This captures both positive recommendations (buy/sell) and explicit warnings against actions"))}, 
    object::pair{std:("conviction"), z->enum(array<string>{ std:("NONE"), std:("LOW"), std:("MEDIUM"), std:("HIGH") })->describe(std:("The level of confidence or urgency expressed in the recommendation, helping prioritize stronger signals"))}
})->describe(std:("Schema for extracting trading recommendations from conversational text, capturing the key components of who made the recommendation, what asset was discussed, what action was recommended, and how strongly it was recommended"));

void Main(void)
{
}

MAIN
