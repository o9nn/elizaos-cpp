#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/communityInvestor/schemas.h"

TokenPerformance transformTokenPerformance(any dbToken, string chain)
{
    auto input = object{
        object::pair{std::string("chain"), std::string("chain")}, 
        object::pair{std::string("address"), OR((dbToken["tokenAddress"]), (dbToken["token_address"]))}, 
        object::pair{std::string("name"), OR((dbToken["name"]), (dbToken["symbol"]))}, 
        object::pair{std::string("symbol"), dbToken["symbol"]}, 
        object::pair{std::string("price"), (type_of(dbToken["price"]) == std::string("number")) ? any(dbToken["price"]) : any(0)}, 
        object::pair{std::string("volume"), (type_of(dbToken["volume"]) == std::string("number")) ? any(dbToken["volume"]) : any(0)}, 
        object::pair{std::string("trades"), (type_of(dbToken["trades"]) == std::string("number")) ? any(dbToken["trades"]) : any(0)}, 
        object::pair{std::string("liquidity"), (type_of(dbToken["liquidity"]) == std::string("number")) ? any(dbToken["liquidity"]) : any(0)}, 
        object::pair{std::string("holders"), (type_of(dbToken["holders"]) == std::string("number")) ? any(dbToken["holders"]) : any(0)}, 
        object::pair{std::string("price24hChange"), (type_of(dbToken["price_change_24h"]) == std::string("number")) ? any(dbToken["price_change_24h"]) : any(0)}, 
        object::pair{std::string("volume24hChange"), (type_of(dbToken["volume_change_24h"]) == std::string("number")) ? any(dbToken["volume_change_24h"]) : any(0)}, 
        object::pair{std::string("trades24hChange"), (type_of(dbToken["trade_24h_change"]) == std::string("number")) ? any(dbToken["trade_24h_change"]) : any(0)}, 
        object::pair{std::string("holders24hChange"), (type_of(dbToken["holder_change_24h"]) == std::string("number")) ? any(dbToken["holder_change_24h"]) : any(0)}, 
        object::pair{std::string("initialMarketCap"), (type_of(dbToken["initial_market_cap"]) == std::string("number")) ? any(dbToken["initial_market_cap"]) : any(0)}, 
        object::pair{std::string("currentMarketCap"), (type_of(dbToken["current_market_cap"]) == std::string("number")) ? any(dbToken["current_market_cap"]) : any(0)}, 
        object::pair{std::string("rugPull"), Boolean(dbToken["rug_pull"])}, 
        object::pair{std::string("isScam"), Boolean(dbToken["is_scam"])}, 
        object::pair{std::string("sustainedGrowth"), Boolean(dbToken["sustained_growth"])}, 
        object::pair{std::string("rapidDump"), Boolean(dbToken["rapid_dump"])}, 
        object::pair{std::string("suspiciousVolume"), Boolean(dbToken["suspicious_volume"])}, 
        object::pair{std::string("validationTrust"), (type_of(dbToken["validation_trust"]) == std::string("number")) ? any(dbToken["validation_trust"]) : any(0)}, 
        object::pair{std::string("createdAt"), (dbToken["created_at"]) ? std::make_shared<Date>(dbToken["created_at"]) : std::make_shared<Date>()}, 
        object::pair{std::string("updatedAt"), (dbToken["updated_at"]) ? std::make_shared<Date>(dbToken["updated_at"]) : std::make_shared<Date>()}
    };
    return tokenPerformanceSchema->parse(input);
};


std::shared_ptr<Transaction> transformTransaction(any dbTx, string positionId, string chain)
{
    auto type = (type_of(dbTx["type"]) == std::string("string")) ? any(dbTx["type"]["toLowerCase"]()) : any(TransactionType["BUY"]);
    auto input = object{
        object::pair{std::string("id"), OR((OR((dbTx["id"]), (dbTx["transaction_hash"]))), (string_empty))}, 
        object::pair{std::string("positionId"), OR((OR((dbTx["positionId"]), (dbTx["position_id"]))), (positionId))}, 
        object::pair{std::string("chain"), OR((dbTx["chain"]), (chain))}, 
        object::pair{std::string("tokenAddress"), OR((dbTx["tokenAddress"]), (dbTx["token_address"]))}, 
        object::pair{std::string("transactionHash"), OR((dbTx["transactionHash"]), (dbTx["transaction_hash"]))}, 
        object::pair{std::string("type"), (OR((type == std::string("BUY")), (type == std::string("SELL")))) ? any(type) : any(std::string("BUY"))}, 
        object::pair{std::string("amount"), (type_of(dbTx["amount"]) == std::string("bigint")) ? any(dbTx["amount"]) : any((type_of(dbTx["amount"]) == std::string("string")) ? BigInt(dbTx["amount"]) : (type_of(dbTx["amount"]) == std::string("number")) ? BigInt(Math->floor(dbTx["amount"])) : BigInt(0))}, 
        object::pair{std::string("price"), (type_of(dbTx["price"]) == std::string("string")) ? any(Number(dbTx["price"])) : any((type_of(dbTx["price"]) == std::string("number")) ? any(dbTx["price"]) : any(undefined))}, 
        object::pair{std::string("isSimulation"), Boolean(OR((dbTx["isSimulation"]), (dbTx["is_simulation"])))}, 
        object::pair{std::string("timestamp"), (is<Date>(dbTx["timestamp"])) ? dbTx["timestamp"]["toISOString"]() : (type_of(dbTx["timestamp"]) == std::string("string")) ? any(dbTx["timestamp"]) : any(((std::make_shared<Date>()))->toISOString())}
    };
    return as<std::shared_ptr<Transaction>>(as<any>(transactionSchema->parse(input)));
};


Position transformPosition(any dbPos)
{
    auto input = object{
        object::pair{std::string("id"), OR((dbPos["id"]), (string_empty))}, 
        object::pair{std::string("chain"), OR((dbPos["chain"]), (std::string("unknown")))}, 
        object::pair{std::string("tokenAddress"), OR((dbPos["tokenAddress"]), (dbPos["token_address"]))}, 
        object::pair{std::string("walletAddress"), OR((dbPos["walletAddress"]), (dbPos["wallet_address"]))}, 
        object::pair{std::string("isSimulation"), Boolean(OR((dbPos["isSimulation"]), (dbPos["is_simulation"])))}, 
        object::pair{std::string("entityId"), OR((dbPos["entityId"]), (dbPos["recommender_id"]))}, 
        object::pair{std::string("recommendationId"), OR((dbPos["recommendationId"]), (dbPos["recommendation_id"]))}, 
        object::pair{std::string("initialPrice"), OR((OR((dbPos["initialPrice"]["toString"]()), (dbPos["initial_price"]["toString"]()))), (std::string("0")))}, 
        object::pair{std::string("initialMarketCap"), OR((OR((dbPos["initialMarketCap"]["toString"]()), (dbPos["initial_market_cap"]["toString"]()))), (std::string("0")))}, 
        object::pair{std::string("initialLiquidity"), OR((OR((dbPos["initialLiquidity"]["toString"]()), (dbPos["initial_liquidity"]["toString"]()))), (std::string("0")))}, 
        object::pair{std::string("performanceScore"), (type_of(dbPos["performanceScore"]) == std::string("number")) ? dbPos["performanceScore"] : (type_of(dbPos["performance_score"]) == std::string("number")) ? any(dbPos["performance_score"]) : any(0)}, 
        object::pair{std::string("rapidDump"), Boolean(OR((dbPos["rapidDump"]), (dbPos["rapid_dump"])))}, 
        object::pair{std::string("openedAt"), (is<Date>(dbPos["openedAt"])) ? dbPos["openedAt"] : (is<Date>(dbPos["opened_at"])) ? any(dbPos["opened_at"]) : any((type_of(dbPos["openedAt"]) == std::string("string")) ? std::make_shared<Date>(dbPos["openedAt"]) : (type_of(dbPos["opened_at"]) == std::string("string")) ? std::make_shared<Date>(dbPos["opened_at"]) : std::make_shared<Date>())}, 
        object::pair{std::string("closedAt"), (is<Date>(dbPos["closedAt"])) ? dbPos["closedAt"] : (is<Date>(dbPos["closed_at"])) ? any(dbPos["closed_at"]) : any((type_of(dbPos["closedAt"]) == std::string("string")) ? std::make_shared<Date>(dbPos["closedAt"]) : (type_of(dbPos["closed_at"]) == std::string("string")) ? any(std::make_shared<Date>(dbPos["closed_at"])) : any(undefined))}, 
        object::pair{std::string("updatedAt"), (is<Date>(dbPos["updatedAt"])) ? dbPos["updatedAt"] : (is<Date>(dbPos["updated_at"])) ? any(dbPos["updated_at"]) : any((type_of(dbPos["updatedAt"]) == std::string("string")) ? std::make_shared<Date>(dbPos["updatedAt"]) : (type_of(dbPos["updated_at"]) == std::string("string")) ? std::make_shared<Date>(dbPos["updated_at"]) : std::make_shared<Date>())}, 
        object::pair{std::string("amount"), OR((dbPos["amount"]["toString"]()), (std::string("0")))}, 
        object::pair{std::string("entryPrice"), OR((OR((dbPos["entryPrice"]["toString"]()), (dbPos["entry_price"]["toString"]()))), (std::string("0")))}, 
        object::pair{std::string("currentPrice"), OR((OR((dbPos["currentPrice"]["toString"]()), (dbPos["current_price"]["toString"]()))), (std::string("0")))}
    };
    return positionSchema->parse(input);
};


TokenRecommendation transformTokenRecommendation(any dbRec)
{
    try
    {
        return tokenRecommendationSchema->parse(object{
            object::pair{std::string("id"), OR((dbRec["id"]), (dbRec["recommendation_id"]))}, 
            object::pair{std::string("entityId"), OR((dbRec["entityId"]), (dbRec["recommender_id"]))}, 
            object::pair{std::string("chain"), OR((dbRec["chain"]), (std::string("unknown")))}, 
            object::pair{std::string("tokenAddress"), OR((dbRec["tokenAddress"]), (dbRec["token_address"]))}, 
            object::pair{std::string("type"), OR((dbRec["type"]), (std::string("BUY")))}, 
            object::pair{std::string("conviction"), OR((dbRec["conviction"]), (std::string("MEDIUM")))}, 
            object::pair{std::string("initialMarketCap"), OR((OR((dbRec["initialMarketCap"]), (dbRec["initial_market_cap"]))), (std::string("0")))}, 
            object::pair{std::string("initialLiquidity"), OR((OR((dbRec["initialLiquidity"]), (dbRec["initial_liquidity"]))), (std::string("0")))}, 
            object::pair{std::string("initialPrice"), OR((OR((dbRec["initialPrice"]), (dbRec["initial_price"]))), (std::string("0")))}, 
            object::pair{std::string("marketCap"), OR((OR((dbRec["marketCap"]), (dbRec["market_cap"]))), (std::string("0")))}, 
            object::pair{std::string("liquidity"), OR((dbRec["liquidity"]), (std::string("0")))}, 
            object::pair{std::string("price"), OR((dbRec["price"]), (std::string("0")))}, 
            object::pair{std::string("rugPull"), Boolean(OR((OR((dbRec["rugPull"]), (dbRec["rug_pull"]))), (false)))}, 
            object::pair{std::string("isScam"), Boolean(OR((OR((dbRec["isScam"]), (dbRec["is_scam"]))), (false)))}, 
            object::pair{std::string("riskScore"), OR((OR((dbRec["riskScore"]), (dbRec["risk_score"]))), (0))}, 
            object::pair{std::string("performanceScore"), OR((OR((dbRec["performanceScore"]), (dbRec["performance_score"]))), (0))}, 
            object::pair{std::string("metadata"), OR((dbRec["metadata"]), (object{}))}, 
            object::pair{std::string("status"), OR((dbRec["status"]), (std::string("ACTIVE")))}, 
            object::pair{std::string("createdAt"), std::make_shared<Date>(OR((OR((dbRec["createdAt"]), (dbRec["created_at"]))), (Date->now())))}, 
            object::pair{std::string("updatedAt"), std::make_shared<Date>(OR((OR((dbRec["updatedAt"]), (dbRec["updated_at"]))), (Date->now())))}
        });
    }
    catch (const any& error)
    {
        console->error(std::string("Error transforming token recommendation:"), error);
        return nullptr;
    }
};


object TransactionType = as<std::shared_ptr<const>>(object{
    object::pair{std::string("BUY"), std::string("BUY")}, 
    object::pair{std::string("SELL"), std::string("SELL")}, 
    object::pair{std::string("TRANSFER_IN"), std::string("transfer_in")}, 
    object::pair{std::string("TRANSFER_OUT"), std::string("transfer_out")}
});
any tokenPerformanceSchema = z->object(object{
    object::pair{std::string("chain"), z->string()->default(std::string("unknown"))}, 
    object::pair{std::string("address"), z->string()}, 
    object::pair{std::string("name"), z->string()->optional()->default(string_empty)}, 
    object::pair{std::string("symbol"), z->string()}, 
    object::pair{std::string("decimals"), z->number()->default(0)}, 
    object::pair{std::string("metadata"), z->record(z->any())->default(object{})}, 
    object::pair{std::string("price"), z->number()->default(0)}, 
    object::pair{std::string("volume"), z->number()->default(0)}, 
    object::pair{std::string("trades"), z->number()->default(0)}, 
    object::pair{std::string("liquidity"), z->number()->default(0)}, 
    object::pair{std::string("holders"), z->number()->default(0)}, 
    object::pair{std::string("price24hChange"), z->number()->default(0)}, 
    object::pair{std::string("volume24hChange"), z->number()->default(0)}, 
    object::pair{std::string("trades24hChange"), z->number()->default(0)}, 
    object::pair{std::string("holders24hChange"), z->number()->default(0)}, 
    object::pair{std::string("initialMarketCap"), z->number()->default(0)}, 
    object::pair{std::string("currentMarketCap"), z->number()->default(0)}, 
    object::pair{std::string("rugPull"), z->boolean()->default(false)}, 
    object::pair{std::string("isScam"), z->boolean()->default(false)}, 
    object::pair{std::string("sustainedGrowth"), z->boolean()->default(false)}, 
    object::pair{std::string("rapidDump"), z->boolean()->default(false)}, 
    object::pair{std::string("suspiciousVolume"), z->boolean()->default(false)}, 
    object::pair{std::string("validationTrust"), z->number()->default(0)}, 
    object::pair{std::string("createdAt"), z->date()->default([=]() mutable
    {
        return std::make_shared<Date>();
    }
    )}, 
    object::pair{std::string("updatedAt"), z->date()->default([=]() mutable
    {
        return std::make_shared<Date>();
    }
    )}
});
any transactionSchema = z->object(object{
    object::pair{std::string("id"), z->string()}, 
    object::pair{std::string("positionId"), z->string()}, 
    object::pair{std::string("chain"), z->string()->default(std::string("unknown"))}, 
    object::pair{std::string("tokenAddress"), z->string()}, 
    object::pair{std::string("transactionHash"), z->string()}, 
    object::pair{std::string("type"), z->enum(array<string>{ TransactionType["BUY"], TransactionType["SELL"] })}, 
    object::pair{std::string("amount"), z->number()}, 
    object::pair{std::string("price"), z->number()->optional()}, 
    object::pair{std::string("isSimulation"), z->boolean()->default(false)}, 
    object::pair{std::string("timestamp"), z->string()}
});
any recommenderMetricsSchema = z->object(object{
    object::pair{std::string("entityId"), z->string()}, 
    object::pair{std::string("trustScore"), z->number()}, 
    object::pair{std::string("totalRecommendations"), z->number()}, 
    object::pair{std::string("successfulRecs"), z->number()}, 
    object::pair{std::string("avgTokenPerformance"), z->number()}, 
    object::pair{std::string("riskScore"), z->number()}, 
    object::pair{std::string("consistencyScore"), z->number()}, 
    object::pair{std::string("virtualConfidence"), z->number()}, 
    object::pair{std::string("lastActiveDate"), z->date()}, 
    object::pair{std::string("trustDecay"), z->number()}, 
    object::pair{std::string("updatedAt"), z->date()->optional()->default([=]() mutable
    {
        return std::make_shared<Date>();
    }
    )}
});
any positionSchema = z->object(object{
    object::pair{std::string("id"), z->string()->uuid()}, 
    object::pair{std::string("chain"), z->string()}, 
    object::pair{std::string("tokenAddress"), z->string()}, 
    object::pair{std::string("walletAddress"), z->string()}, 
    object::pair{std::string("isSimulation"), z->boolean()}, 
    object::pair{std::string("entityId"), z->string()}, 
    object::pair{std::string("recommendationId"), z->string()}, 
    object::pair{std::string("initialPrice"), z->string()}, 
    object::pair{std::string("initialMarketCap"), z->string()}, 
    object::pair{std::string("initialLiquidity"), z->string()}, 
    object::pair{std::string("performanceScore"), z->number()}, 
    object::pair{std::string("rapidDump"), z->boolean()}, 
    object::pair{std::string("openedAt"), z->date()}, 
    object::pair{std::string("closedAt"), z->date()->optional()}, 
    object::pair{std::string("updatedAt"), z->date()}, 
    object::pair{std::string("amount"), z->string()}, 
    object::pair{std::string("entryPrice"), z->string()}, 
    object::pair{std::string("currentPrice"), z->string()}
});
any tokenRecommendationSchema = z->object(object{
    object::pair{std::string("id"), z->string()->uuid()}, 
    object::pair{std::string("entityId"), z->string()->uuid()}, 
    object::pair{std::string("chain"), z->string()}, 
    object::pair{std::string("tokenAddress"), z->string()}, 
    object::pair{std::string("type"), z->string()}, 
    object::pair{std::string("conviction"), z->string()}, 
    object::pair{std::string("initialMarketCap"), z->string()}, 
    object::pair{std::string("initialLiquidity"), z->string()}, 
    object::pair{std::string("initialPrice"), z->string()}, 
    object::pair{std::string("marketCap"), z->string()}, 
    object::pair{std::string("liquidity"), z->string()}, 
    object::pair{std::string("price"), z->string()}, 
    object::pair{std::string("rugPull"), z->boolean()}, 
    object::pair{std::string("isScam"), z->boolean()}, 
    object::pair{std::string("riskScore"), z->number()}, 
    object::pair{std::string("performanceScore"), z->number()}, 
    object::pair{std::string("metadata"), z->record(z->any())->default(object{})}, 
    object::pair{std::string("status"), z->enum(array<string>{ std::string("ACTIVE"), std::string("COMPLETED"), std::string("EXPIRED"), std::string("WITHDRAWN") })}, 
    object::pair{std::string("createdAt"), z->date()}, 
    object::pair{std::string("updatedAt"), z->date()}
});
any recommendationSchema = z->object(object{
    object::pair{std::string("username"), z->string()->describe(std::string("The username of the person making the recommendation in the conversation"))}, 
    object::pair{std::string("ticker"), z->string()->optional()->nullable()->describe(std::string("The ticker symbol of the recommended asset (e.g., 'BTC', 'AAPL'). Optional as recommendations may discuss assets without explicit tickers"))}, 
    object::pair{std::string("tokenAddress"), z->string()->optional()->nullable()->describe(std::string("The blockchain contract address of the token if mentioned. This helps disambiguate tokens that might share similar names or symbols"))}, 
    object::pair{std::string("type"), z->enum(array<string>{ std::string("BUY"), std::string("SELL"), std::string("DONT_BUY"), std::string("DONT_SELL"), std::string("NONE") })->describe(std::string("The type of trading recommendation being made. This captures both positive recommendations (buy/sell) and explicit warnings against actions"))}, 
    object::pair{std::string("conviction"), z->enum(array<string>{ std::string("NONE"), std::string("LOW"), std::string("MEDIUM"), std::string("HIGH") })->describe(std::string("The level of confidence or urgency expressed in the recommendation, helping prioritize stronger signals"))}
})->describe(std::string("Schema for extracting trading recommendations from conversational text, capturing the key components of who made the recommendation, what asset was discussed, what action was recommended, and how strongly it was recommended"));

void Main(void)
{
}

MAIN
