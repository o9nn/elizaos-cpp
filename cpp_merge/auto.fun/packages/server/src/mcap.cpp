#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/mcap.h"

std::shared_ptr<Promise<double>> getSOLPrice()
{
    auto cacheService = std::make_shared<CacheService>();
    auto cachedPrice = std::async([=]() { cacheService->getSolPrice(); });
    console->log(std:("cachedPrice"), cachedPrice);
    if (AND((AND((Number->isNaN(cachedPrice) != false), (cachedPrice != nullptr))), (cachedPrice > 0))) {
        return cachedPrice;
    }
    try
    {
        auto response = std::async([=]() { fetch(std:("https://api.coingecko.com/api/v3/simple/price?ids=solana&vs_currencies=usd")); });
        auto data = as<any>((std::async([=]() { response->json(); })));
        if (AND((AND((data), (data["solana"]))), (data["solana"]["usd"]))) {
            auto price = data["solana"]["usd"];
            auto cacheService = std::make_shared<CacheService>();
            std::async([=]() { cacheService->setSolPrice(price); });
            return price;
        }
    }
    catch (const any& error)
    {
        logger["error"](std:("Error fetching SOL price from Coingecko:"), error);
    }
    try
    {
        auto response = std::async([=]() { fetch(std:("https://api.binance.com/api/v3/ticker/price?symbol=SOLUSDT")); });
        auto data = as<any>((std::async([=]() { response->json(); })));
        if (AND((data), (data["price"]))) {
            auto price = parseFloat(data["price"]);
            auto cacheService = std::make_shared<CacheService>();
            std::async([=]() { cacheService->setSolPrice(price); });
            return price;
        }
    }
    catch (const any& error)
    {
        logger["error"](std:("Error fetching SOL price from Binance:"), error);
    }
    return 135;
};


std::shared_ptr<Promise<double>> fetchSOLPriceFromPyth()
{
    try
    {
        auto pythConnection = std::make_shared<Connection>(getPythClusterApiUrl(PYTHNET_CLUSTER_NAME));
        auto pythPublicKey = getPythProgramKeyForCluster(PYTHNET_CLUSTER_NAME);
        auto pythClient = std::make_shared<PythHttpClient>(as<any>(pythConnection), pythPublicKey);
        auto data = std::async([=]() { pythClient->getData(); });
        auto solPrice = data->productPrice->get(SOLUSD_SYMBOL);
        if (OR((!solPrice), (!solPrice->price))) {
            logger["error"](std:("Unable to get SOL/USD price from Pyth"));
            return 0;
        }
        return solPrice->price;
    }
    catch (const any& error)
    {
        logger["error"](std:("Error fetching SOL price from Pyth:"), error);
        return 0;
    }
};


std::shared_ptr<Promise<any>> calculateTokenMarketData(any token, double solPrice)
{
    auto tokenWithMarketData = utils::assign(object{
    }, token);
    if (token["currentPrice"]) {
        tokenWithMarketData["tokenPriceUSD"] = token["currentPrice"] * solPrice;
    }
    if (shouldUpdateSupply(token)) {
        auto updatedSupply = std::async([=]() { updateTokenSupplyFromChain(token["mint"]); });
        tokenWithMarketData["tokenSupply"] = updatedSupply["tokenSupply"];
        tokenWithMarketData["tokenSupplyUiAmount"] = updatedSupply["tokenSupplyUiAmount"];
        tokenWithMarketData["tokenDecimals"] = updatedSupply["tokenDecimals"];
        tokenWithMarketData["lastSupplyUpdate"] = updatedSupply["lastSupplyUpdate"];
    }
    if (token["tokenSupplyUiAmount"]) {
        if (tokenWithMarketData["tokenPriceUSD"]) {
            tokenWithMarketData["marketCapUSD"] = token["tokenSupplyUiAmount"] * tokenWithMarketData["tokenPriceUSD"];
        }
    }
    tokenWithMarketData["solPriceUSD"] = solPrice;
    return tokenWithMarketData;
};


any calculateRaydiumTokenMarketData(any token)
{
    try
    {
        auto TOKEN_DECIMALS = Number(OR((process->env->DECIMALS), (6)));
        auto solPrice = std::async([=]() { getSOLPrice(); });
        auto raydium = std::async([=]() { initSdk(object{
            object::pair{std:("loadToken"), true}
        }); });
        any poolInfo;
        shared retries = 5;
        while (retries > 0)
        {
            try
            {
                if (raydium->cluster == std:("devnet")) {
                    auto data = std::async([=]() { raydium->cpmm->getPoolInfoFromRpc(token["marketId"]); });
                    poolInfo = data->poolInfo;
                } else {
                    auto data = std::async([=]() { raydium->api->fetchPoolById(object{
                        object::pair{std:("ids"), token["marketId"]}
                    }); });
                    if (OR((!data), (data->length == 0))) {
                        logger["error"](std:("Mcap: Pool info not found"));
                        throw any(std::make_shared<Error>(std:("Mcap: Pool info not found")));
                    }
                    poolInfo = const_(data)[0];
                }
                break;
            }
            catch (const any& error)
            {
                retries--;
                if (retries == 0) {
                    logger["error"](std:("Mcap: Failed to fetch pool info after retries: ") + (is<Error>(error)) ? any(error->message) (std:("Unknown error")) + string_empty);
                }
                std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                {
                    return setTimeout(resolve, (5 - retries) * 5000);
                }
                ); });
            }
        }
        if (OR((OR((!poolInfo), (!poolInfo["mintAmountA"]))), (!poolInfo["mintAmountB"]))) {
            logger["error"](std:("Mcap: Invalid pool info structure"));
        }
        if (!poolInfo) {
            throw any(std::make_shared<Error>(std:("Mcap: Invalid pool info structure")));
        }
        if (OR((!poolInfo["mintAmountA"]), (!poolInfo["mintAmountB"]))) {
            throw any(std::make_shared<Error>(std:("Mcap: Invalid pool info structure")));
        }
        if (OR((type_of(poolInfo["mintAmountA"]) != std:("number")), (type_of(poolInfo["mintAmountB"]) != std:("number")))) {
            throw any(std::make_shared<Error>(std:("Mcap: Invalid pool info structure")));
        }
        auto currentPrice = (poolInfo["mintAmountA"] > 0) ? any(poolInfo["mintAmountA"] / poolInfo["mintAmountB"]) (0);
        auto tokenPriceUSD = (currentPrice > 0) ? any(currentPrice * solPrice) (0);
        auto marketCapUSD = (Number(process->env->TOKEN_SUPPLY) / Math->pow(10, TOKEN_DECIMALS)) * tokenPriceUSD;
        if (marketCapUSD < 0) {
            throw any(std::make_shared<Error>(std:("Mcap: Market cap is negative")));
        }
        if (OR((!poolInfo["mintAmountA"]), (!poolInfo["mintAmountB"]))) {
            throw any(std::make_shared<Error>(std:("Mcap: Invalid pool info structure")));
        }
        auto liquidity = (AND((poolInfo["mintAmountA"] > 0), (poolInfo["mintAmountB"] > 0))) ? any(poolInfo["mintAmountB"] * tokenPriceUSD + poolInfo["mintAmountA"] * solPrice) (0);
        return object{
            object::pair{std:("marketCapUSD"), std:("marketCapUSD")}, 
            object::pair{std:("tokenPriceUSD"), std:("tokenPriceUSD")}, 
            object::pair{std:("solPriceUSD"), solPrice}, 
            object::pair{std:("currentPrice"), std:("currentPrice")}, 
            object::pair{std:("liquidity"), std:("liquidity")}
        };
    }
    catch (const any& error)
    {
        logger["error"](std:("Error calculating Raydium token market data for ") + token["mint"] + std:(":"), error);
        logger["error"](std:("RPC Node issue - Consider using a paid RPC endpoint for better reliability"));
        failedUpdates++;
        return object{
            object::pair{std:("marketCapUSD"), 0}, 
            object::pair{std:("tokenPriceUSD"), 0}, 
            object::pair{std:("solPriceUSD"), 0}, 
            object::pair{std:("currentPrice"), 0}, 
            object::pair{std:("liquidity"), 0}
        };
    }
};


any getMarketDataMetrics()
{
    return object{
        object::pair{std:("totalUpdatesProcessed"), std:("totalUpdatesProcessed")}, 
        object::pair{std:("failedUpdates"), std:("failedUpdates")}, 
        object::pair{std:("lastUpdateTime"), std:("lastUpdateTime")}
    };
};


std::shared_ptr<PythCluster> PYTHNET_CLUSTER_NAME = std:("pythnet");
string SOLUSD_SYMBOL = std:("Crypto.SOL/USD");
double MAX_CONCURRENT_TOKENS = 3;
double totalUpdatesProcessed = 0;
double failedUpdates = 0;
any lastUpdateTime = nullptr;

void Main(void)
{
}

MAIN
