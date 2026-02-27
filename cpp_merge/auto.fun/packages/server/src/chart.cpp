#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/chart.h"

any getLatestCandle(string tokenMint, any swap, any tokenInfo)
{
    auto swapTime = ((std::make_shared<Date>(swap["timestamp"])))->getTime() / 1000;
    auto candlePeriod = 60;
    auto candleStart = Math->floor(swapTime / candlePeriod) * candlePeriod;
    auto db = getDB();
    if (!tokenInfo) {
        tokenInfo = std::async([=]() { db["select"]()["from"](tokens)["where"](eq(tokens->mint, tokenMint))["limit"](1); });
    }
    if (tokenInfo["status"] == std:("locked")) {
        try
        {
            auto tokenAddress = (process->env->NETWORK == std:("devnet")) ? any(DEV_TEST_TOKEN_ADDRESS) (tokenMint);
            auto candles = std::async([=]() { fetchCodexBars(tokenAddress, candleStart, candleStart + candlePeriod, std:("1"), undefined, undefined); });
            if (candles->get_length() > 0) {
                auto idx = candles->get_length() - 1;
                auto lastRaw = const_(candles)[idx];
                auto lastCandle = utils::assign(object{
                    , 
                    object::pair{std:("price"), lastRaw->close}, 
                    object::pair{std:("timestamp"), std::make_shared<Date>(lastRaw->time * 1000)}
                }, lastRaw);
                return lastCandle;
            }
        }
        catch (const any& error)
        {
            logger["error"](std:("Error fetching latest candle from Codex:"), error);
        }
    }
    auto latestCandle = std::async([=]() { fetchPriceChartData(candleStart * 1000, (candleStart + candlePeriod) * 1000, 1, tokenMint); });
    return (AND((latestCandle), (latestCandle->get_length() > 0))) ? any(const_(latestCandle)[0]) (nullptr);
};


any fetchPriceChartData(double start, double end, double range, string tokenMint)
{
    auto db = getDB();
    auto [tokenInfo] = std::async([=]() { db["select"]()["from"](tokens)["where"](eq(tokens->mint, tokenMint))["limit"](1); });
    if (!tokenInfo) {
        logger["error"](std:("Token ") + tokenMint + std:(" not found"));
        return array<any>();
    }
    if (tokenInfo["status"] != std:("locked")) {
        auto swapRecordsRaw = array<any>();
        try
        {
            auto redisCache = std::async([=]() { getGlobalRedisCache(); });
            auto listKey = std:("swapsList:") + tokenMint + string_empty;
            auto swapStrings = std::async([=]() { redisCache->lrange(listKey, 0, -1); });
            swapRecordsRaw = swapStrings->map([=](auto s) mutable
            {
                return JSON->parse(s);
            }
            );
            logger["log"](std:("Chart: Retrieved ") + swapRecordsRaw->get_length() + std:(" raw swaps from Redis list ") + listKey + string_empty);
        }
        catch (const any& redisError)
        {
            logger["error"](std:("Chart: Failed to read swaps from Redis list swapsList:") + tokenMint + std:(":"), redisError);
            return array<any>();
        }
        shared solPrice = std::async([=]() { getSOLPrice(); });
        auto filteredSwaps = swapRecordsRaw->filter([=](auto swap) mutable
        {
            auto swapTime = ((std::make_shared<Date>(swap["timestamp"])))->getTime();
            return AND((swapTime >= start), (swapTime <= end));
        }
        );
        auto priceFeeds = filteredSwaps->filter([=](auto swap) mutable
        {
            return AND((not_equals(swap["price"], nullptr)), (not_equals(swap["timestamp"], nullptr)));
        }
        )->map([=](auto swap) mutable
        {
            return (object{
                object::pair{std:("price"), OR((swap["priceUsd"]), (swap["price"]))}, 
                object::pair{std:("priceUsd"), OR((swap["priceUsd"]), (undefined))}, 
                object::pair{std:("timestamp"), std::make_shared<Date>(swap["timestamp"])}, 
                object::pair{std:("volume"), (swap["direction"] == 0) ? (OR((swap["amountIn"]), (0))) / 1000000000 : (OR((swap["amountOut"]), (0))) / 1000000000}
            });
        }
        );
        priceFeeds->forEach([=](auto feed) mutable
        {
            if (!feed->priceUsd) {
                feed->price = feed->price * solPrice;
            }
        }
        );
        if (!priceFeeds->get_length()) return array<any>();
        auto cdFeeds = getCandleData(priceFeeds, range);
        return cdFeeds;
    } else if (tokenInfo["status"] == std:("locked")) {
        try
        {
            auto tokenAddress = (process->env->NETWORK == std:("devnet")) ? any(DEV_TEST_TOKEN_ADDRESS) (tokenMint);
            auto resolution = std:("1");
            switch (static_cast<size_t>(range))
            {
            case 1:
                resolution = std:("1");
                break;
            case 5:
                resolution = std:("5");
                break;
            case 15:
                resolution = std:("15");
                break;
            case 60:
                resolution = std:("60");
                break;
            case 120:
                resolution = std:("60");
                break;
            default:
                resolution = std:("1");
            }
            auto candles = std::async([=]() { fetchCodexBars(tokenAddress, Math->floor(start / 1000), Math->floor(end / 1000), resolution, undefined, undefined); });
            if (AND((range == 120), (candles->get_length() > 1))) {
                auto combined = array<any>();
                for (auto i = 0; i < candles->get_length(); i += 2)
                {
                    if (i + 1 < candles->get_length()) {
                        auto out = object{
                            object::pair{std:("open"), const_(candles)[i]->open}, 
                            object::pair{std:("high"), Math->max(const_(candles)[i]->high, const_(candles)[i + 1]->high)}, 
                            object::pair{std:("low"), Math->min(const_(candles)[i]->low, const_(candles)[i + 1]->low)}, 
                            object::pair{std:("close"), const_(candles)[i + 1]->close}, 
                            object::pair{std:("volume"), const_(candles)[i]->volume + const_(candles)[i + 1]->volume}, 
                            object::pair{std:("time"), const_(candles)[i]->time}, 
                            object::pair{std:("price"), const_(candles)[i]->close}, 
                            object::pair{std:("timestamp"), std::make_shared<Date>(const_(candles)[i]->time * 1000)}
                        };
                        combined->push(out);
                    } else {
                        auto lastCandle = utils::assign(object{
                            , 
                            object::pair{std:("price"), const_(candles)[i]->close}, 
                            object::pair{std:("timestamp"), std::make_shared<Date>(const_(candles)[i]->time * 1000)}
                        }, const_(candles)[i]);
                        combined->push(lastCandle);
                    }
                }
                return combined;
            }
            return candles;
        }
        catch (const any& error)
        {
            logger["error"](std:("Error fetching data with getBars API:"), error);
            try
            {
                logger["log"](std:("Falling back to getTokenEvents API"));
                auto tokenAddress = (process->env->NETWORK == std:("devnet")) ? any(DEV_TEST_TOKEN_ADDRESS) (tokenMint);
                auto tokenEvents = std::async([=]() { fetchCodexTokenEvents(tokenAddress, Math->floor(start / 1000), Math->floor(end / 1000), 1399811149); });
                auto priceFeeds = tokenEvents->filter([=](auto item) mutable
                {
                    return not_equals(item->timestamp, nullptr);
                }
                )->map([=](auto item) mutable
                {
                    return (object{
                        object::pair{std:("price"), parseFloat(OR((item->token1PoolValueUsd), (std:("0"))))}, 
                        object::pair{std:("timestamp"), std::make_shared<Date>(item->timestamp * 1000)}, 
                        object::pair{std:("volume"), parseFloat(OR((item->data->amount0), (std:("0"))))}
                    });
                }
                );
                if (!priceFeeds->get_length()) return array<any>();
                auto cdFeeds = getCandleData(priceFeeds, range);
                return cdFeeds;
            }
            catch (const any& fallbackError)
            {
                logger["error"](std:("Fallback method also failed:"), fallbackError);
                return array<any>();
            }
        }
    }
};


std::shared_ptr<Promise<array<any>>> fetchLockedTokenChartData(string token, double start, double end, double range)
{
    try
    {
        auto codexApiUrl = std:("https://api.dexscreener.com/latest/dex/tokens/") + token + string_empty;
        auto response = std::async([=]() { fetch(codexApiUrl); });
        if (!response->ok) {
            throw any(std::make_shared<Error>(std:("Codex API error: ") + response->status + std:(" ") + response->statusText + string_empty));
        }
        auto data = as<std::shared_ptr<DexScreenerResponse>>((std::async([=]() { response->json(); })));
        if (OR((OR((!data), (!data->pairs))), (data->pairs->get_length() == 0))) {
            logger["error"](std:("No pairs found for token ") + token + string_empty);
            return array<any>();
        }
        auto pairs = data->pairs->sort([=](auto a, auto b) mutable
        {
            return parseFloat(OR((b->liquidity->usd), (std:("0")))) - parseFloat(OR((a->liquidity->usd), (std:("0"))));
        }
        );
        auto mainPair = const_(pairs)[0];
        if (OR((!mainPair), (!mainPair->priceUsd))) {
            logger["error"](std:("No price data found for token ") + token + string_empty);
            return array<any>();
        }
        auto chartApiUrl = std:("https://api.dexscreener.com/latest/dex/charts/solana/") + mainPair->pairAddress + string_empty;
        auto chartResponse = std::async([=]() { fetch(chartApiUrl); });
        if (!chartResponse->ok) {
            throw any(std::make_shared<Error>(std:("Chart API error: ") + chartResponse->status + std:(" ") + chartResponse->statusText + string_empty));
        }
        auto chartData = as<std::shared_ptr<ChartResponse>>((std::async([=]() { chartResponse->json(); })));
        if (OR((OR((!chartData), (!chartData->priceCandles))), (chartData->priceCandles->get_length() == 0))) {
            logger["error"](std:("No candle data found for pair ") + mainPair->pairAddress + string_empty);
            return array<any>();
        }
        auto candles = chartData->priceCandles->filter([=](auto candle) mutable
        {
            auto candleTime = candle->time * 1000;
            return AND((candleTime >= start), (candleTime <= end));
        }
        )->map([=](auto candle) mutable
        {
            return (object{
                object::pair{std:("open"), parseFloat(candle->open)}, 
                object::pair{std:("high"), parseFloat(candle->high)}, 
                object::pair{std:("low"), parseFloat(candle->low)}, 
                object::pair{std:("close"), parseFloat(candle->close)}, 
                object::pair{std:("volume"), parseFloat(candle->volume)}, 
                object::pair{std:("time"), candle->time}
            });
        }
        );
        if (range > 0) {
            return groupCandlesByRange(candles, range);
        }
        return candles;
    }
    catch (const any& error)
    {
        logger["error"](std:("Error fetching locked token chart data: ") + error + string_empty);
        return array<any>();
    }
};


array<std::shared_ptr<Candle>> groupCandlesByRange(array<std::shared_ptr<Candle>> candles, double rangeMinutes)
{
    if (candles->get_length() == 0) return array<any>();
    auto rangeMs = rangeMinutes * 60 * 1000;
    auto groupedCandles = array<std::shared_ptr<Candle>>();
    auto sortedCandles = (array<std::shared_ptr<Candle>>{ candles })->sort([=](auto a, auto b) mutable
    {
        return a->time - b->time;
    }
    );
    auto currentGroup = array<std::shared_ptr<Candle>>();
    auto currentRangeStart = (Math->floor((const_(sortedCandles)[0]->time * 1000) / rangeMs) * rangeMs) / 1000;
    auto& __array15854_16919 = sortedCandles;
    for (auto __indx15854_16919 = 0_N; __indx15854_16919 < __array15854_16919->get_length(); __indx15854_16919++)
    {
        auto& candle = const_(__array15854_16919)[__indx15854_16919];
        {
            auto candleRangeStart = (Math->floor((candle->time * 1000) / rangeMs) * rangeMs) / 1000;
            if (candleRangeStart == currentRangeStart) {
                currentGroup->push(candle);
            } else {
                if (currentGroup->get_length() > 0) {
                    groupedCandles->push(createCandleFromGroup(currentGroup, currentRangeStart));
                }
                while (candleRangeStart > currentRangeStart + rangeMs / 1000)
                {
                    currentRangeStart += rangeMs / 1000;
                    auto previousCandle = const_(groupedCandles)[groupedCandles->get_length() - 1];
                    groupedCandles->push(object{
                        object::pair{std:("open"), previousCandle->close}, 
                        object::pair{std:("high"), previousCandle->close}, 
                        object::pair{std:("low"), previousCandle->close}, 
                        object::pair{std:("close"), previousCandle->close}, 
                        object::pair{std:("volume"), 0}, 
                        object::pair{std:("time"), currentRangeStart}
                    });
                }
                currentRangeStart = candleRangeStart;
                currentGroup = array<std::shared_ptr<Candle>>{ candle };
            }
        }
    }
    if (currentGroup->get_length() > 0) {
        groupedCandles->push(createCandleFromGroup(currentGroup, currentRangeStart));
    }
    return groupedCandles;
};


std::shared_ptr<Candle> createCandleFromGroup(array<std::shared_ptr<Candle>> group, double rangeStart)
{
    auto open = const_(group)[0]->open;
    auto close = const_(group)[group->get_length() - 1]->close;
    shared high = Math->max(const_(group->map([=](auto c) mutable
    {
        return c->high;
    }
    ))[0]);
    shared low = Math->min(const_(group->map([=](auto c) mutable
    {
        return c->low;
    }
    ))[0]);
    shared volume = group->reduce([=](auto sum, auto c) mutable
    {
        return sum + c->volume;
    }
    , 0);
    return object{
        object::pair{std:("open"), std:("open")}, 
        object::pair{std:("high"), std:("high")}, 
        object::pair{std:("low"), std:("low")}, 
        object::pair{std:("close"), std:("close")}, 
        object::pair{std:("volume"), std:("volume")}, 
        object::pair{std:("time"), rangeStart}
    };
};


string DEV_TEST_TOKEN_ADDRESS = std:("ANNTWQsQ9J3PeM6dXLjdzwYcSzr51RREWQnjuuCEpump");
std::function<array<std::shared_ptr<CandlePrice>>(array<std::shared_ptr<PriceFeedInfo>>, double)> getCandleData = [=](auto priceFeeds, auto range) mutable
{
    auto priceHistory = priceFeeds->map([=](auto feed) mutable
    {
        return (object{
            object::pair{std:("price"), feed->price}, 
            object::pair{std:("ts"), feed->timestamp->getTime() / 1000}
        });
    }
    )->sort([=](auto price1, auto price2) mutable
    {
        return price1["ts"] - price2["ts"];
    }
    );
    if (!priceHistory->get_length()) return array<any>();
    auto candlePeriod = 60;
    switch (static_cast<size_t>(range))
    {
    case 1:
        break;
    case 5:
        candlePeriod = 300;
        break;
    case 15:
        candlePeriod = 900;
        break;
    case 60:
        candlePeriod = 3600;
        break;
    case 120:
        candlePeriod = 7200;
        break;
    }
    auto cdStart = Math->floor(const_(priceHistory)[0]["ts"] / candlePeriod) * candlePeriod;
    auto cdEnd = Math->floor(const_(priceHistory)[priceHistory->get_length() - 1]["ts"] / candlePeriod) * candlePeriod;
    auto cdFeeds = array<CandlePrice>();
    auto pIndex = 0;
    auto lastValidCandle = nullptr;
    for (auto curCdStart = cdStart; curCdStart <= cdEnd; curCdStart += candlePeriod)
    {
        auto st = const_(priceHistory)[pIndex]["price"];
        auto hi = const_(priceHistory)[pIndex]["price"];
        auto lo = const_(priceHistory)[pIndex]["price"];
        auto en = const_(priceHistory)[pIndex]["price"];
        auto vol = 0;
        auto prevIndex = pIndex;
        for (; pIndex < priceHistory->get_length(); )
        {
            if (hi < const_(priceHistory)[pIndex]["price"]) hi = const_(priceHistory)[pIndex]["price"];
            if (lo > const_(priceHistory)[pIndex]["price"]) lo = const_(priceHistory)[pIndex]["price"];
            en = const_(priceHistory)[pIndex]["price"];
            vol += const_(priceFeeds)[pIndex]->volume;
            if (const_(priceHistory)[pIndex]["ts"] >= curCdStart + candlePeriod) break;
            pIndex++;
        }
        if (prevIndex != pIndex) {
            auto newCandle = object{
                object::pair{std:("open"), st}, 
                object::pair{std:("high"), hi}, 
                object::pair{std:("low"), lo}, 
                object::pair{std:("close"), en}, 
                object::pair{std:("volume"), vol}, 
                object::pair{std:("time"), curCdStart}
            };
            cdFeeds->push(newCandle);
            lastValidCandle = newCandle;
        } else if (lastValidCandle) {
            cdFeeds->push(object{
                object::pair{std:("open"), lastValidCandle["close"]}, 
                object::pair{std:("high"), lastValidCandle["close"]}, 
                object::pair{std:("low"), lastValidCandle["close"]}, 
                object::pair{std:("close"), lastValidCandle["close"]}, 
                object::pair{std:("volume"), 0}, 
                object::pair{std:("time"), curCdStart}
            });
        }
    }
    return cdFeeds;
};

void Main(void)
{
}

MAIN
