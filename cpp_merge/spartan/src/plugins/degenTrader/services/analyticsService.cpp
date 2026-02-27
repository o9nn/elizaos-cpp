#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/analyticsService.h"

AnalyticsService::AnalyticsService(std::shared_ptr<IAgentRuntime> runtime_) : runtime(runtime_)  {
}

std::shared_ptr<Promise<void>> AnalyticsService::initialize()
{
    logger->info(std:("Initializing analytics service"));
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> AnalyticsService::stop()
{
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<double>> AnalyticsService::scoreTechnicalSignals(any signals)
{
    if (!signals) return 0;
    auto score = 0;
    if (signals["rsi"] < 30) score += 10; else if (signals["rsi"] > 70) score -= 5; else score += 5;
    if (AND((signals["macd"]["value"] > 0), (signals["macd"]["value"] > signals["macd"]["signal"]))) {
        score += 10;
    } else if (AND((signals["macd"]["value"] < 0), (Math->abs(signals["macd"]["value"]) > Math->abs(signals["macd"]["signal"])))) {
        score -= 5;
    }
    if (AND((signals["volumeProfile"]["trend"] == std:("increasing")), (!signals["volumeProfile"]["unusualActivity"]))) {
        score += 10;
    }
    if (signals["volatility"] < 0.2) score += 10; else if (signals["volatility"] > 0.5) score -= 5;
    return score;
}

std::shared_ptr<Promise<double>> AnalyticsService::scoreSocialMetrics(any metrics)
{
    if (!metrics) return 0;
    auto score = 0;
    auto mentionScore = Math->min(metrics["mentionCount"] / 100, 10);
    score += mentionScore;
    score += metrics["sentiment"] * 10;
    auto influencerScore = Math->min(metrics["influencerMentions"] * 2, 10);
    score += influencerScore;
    return Math->max(0, score);
}

std::shared_ptr<Promise<double>> AnalyticsService::scoreMarketMetrics(object metrics)
{
    auto score = 0;
    if (metrics["marketCap"] > 1000000000) score += 2; else if (metrics["marketCap"] > 100000000) score += 5; else if (metrics["marketCap"] > 10000000) score += 10; else score += 3;
    auto volumeToMcap = metrics["volume24h"] / metrics["marketCap"];
    score += Math->min(volumeToMcap * 100, 10);
    auto liquidityToMcap = metrics["liquidity"] / metrics["marketCap"];
    score += Math->min(liquidityToMcap * 100, 10);
    return score;
}

std::shared_ptr<Promise<void>> AnalyticsService::trackSlippageImpact(string tokenAddress, string expectedAmount, string actualAmount, double slippageBps, boolean isSell)
{
    try
    {
        auto expected = Number(expectedAmount);
        auto actual = Number(actualAmount);
        if (OR((expected <= 0), (actual <= 0))) {
            logger->warn(std:("Invalid amounts for slippage tracking"), object{
                object::pair{std:("tokenAddress"), std:("tokenAddress")}, 
                object::pair{std:("expectedAmount"), std:("expectedAmount")}, 
                object::pair{std:("actualAmount"), std:("actualAmount")}
            });
            return std::shared_ptr<Promise<void>>();
        }
        auto actualSlippage = ((expected - actual) / expected) * 100;
        auto actualSlippageBps = Math->floor(actualSlippage * 100);
        std::async([=]() { this->runtime->setCache(std:("slippage_impact:") + tokenAddress + std:(":") + Date->now() + string_empty, object{
            object::pair{std:("tokenAddress"), std:("tokenAddress")}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}, 
            object::pair{std:("expectedAmount"), std:("expectedAmount")}, 
            object::pair{std:("actualAmount"), std:("actualAmount")}, 
            object::pair{std:("slippageBpsUsed"), slippageBps}, 
            object::pair{std:("actualSlippageBps"), std:("actualSlippageBps")}, 
            object::pair{std:("isSell"), std:("isSell")}
        }); });
        logger->info(std:("Trade slippage impact tracked"), object{
            object::pair{std:("tokenAddress"), std:("tokenAddress")}, 
            object::pair{std:("slippageBpsUsed"), slippageBps}, 
            object::pair{std:("actualSlippageBps"), std:("actualSlippageBps")}, 
            object::pair{std:("efficiency"), actualSlippageBps / slippageBps}
        });
    }
    catch (const any& error)
    {
        console->log(std:("Error tracking slippage impact"), error);
    }
}

double AnalyticsService::calculateRSI(array<double> prices, double period)
{
    if (prices->get_length() < period + 1) {
        return 50;
    }
    auto gains = 0;
    auto losses = 0;
    for (auto i = 1; i <= period; i++)
    {
        auto change = const_(prices)[i] - const_(prices)[i - 1];
        if (change >= 0) {
            gains += change;
        } else {
            losses -= change;
        }
    }
    auto avgGain = gains / period;
    auto avgLoss = losses / period;
    for (auto i = period + 1; i < prices->get_length(); i++)
    {
        auto change = const_(prices)[i] - const_(prices)[i - 1];
        if (change >= 0) {
            avgGain = (avgGain * (period - 1) + change) / period;
            avgLoss = (avgLoss * (period - 1)) / period;
        } else {
            avgGain = (avgGain * (period - 1)) / period;
            avgLoss = (avgLoss * (period - 1) - change) / period;
        }
    }
    auto rs = avgGain / avgLoss;
    return 100 - 100 / (1 + rs);
}

object AnalyticsService::calculateMACD(array<double> prices)
{
    auto shortPeriod = 12;
    auto longPeriod = 26;
    auto signalPeriod = 9;
    if (prices->get_length() < longPeriod) {
        return object{
            object::pair{std:("macd"), 0}, 
            object::pair{std:("signal"), 0}, 
            object::pair{std:("histogram"), 0}
        };
    }
    auto shortEMA = this->calculateEMA(prices, shortPeriod);
    auto longEMA = this->calculateEMA(prices, longPeriod);
    auto macdLine = shortEMA - longEMA;
    auto signalLine = this->calculateEMA(array<double>{ macdLine }, signalPeriod);
    auto histogram = macdLine - signalLine;
    return object{
        object::pair{std:("macd"), macdLine}, 
        object::pair{std:("signal"), signalLine}, 
        object::pair{std:("histogram"), std:("histogram")}
    };
}

double AnalyticsService::calculateEMA(array<double> prices, double period)
{
    if (prices->get_length() < period) {
        return const_(prices)[prices->get_length() - 1];
    }
    auto multiplier = 2 / (period + 1);
    auto ema = prices->slice(0, period)->reduce([=](auto sum, auto price) mutable
    {
        return sum + price;
    }
    , 0) / period;
    for (auto i = period; i < prices->get_length(); i++)
    {
        ema = (const_(prices)[i] - ema) * multiplier + ema;
    }
    return ema;
}

std::shared_ptr<Promise<void>> AnalyticsService::trackTradeExecution(object data)
{
    try
    {
        auto tradeData = utils::assign(object{
            object::pair{std:("id"), uuidv4()}, 
            object::pair{std:("timestamp"), ((std::make_shared<Date>()))->toISOString()}
        }, data);
        std::async([=]() { this->runtime->setCache(std:("trade_execution:") + tradeData["id"] + string_empty, tradeData); });
        logger->info(std:("Trade execution tracked: ") + data["type"] + string_empty, object{
            object::pair{std:("tokenAddress"), data["tokenAddress"]}, 
            object::pair{std:("amount"), data["amount"]}
        });
    }
    catch (const any& error)
    {
        console->log(std:("Error tracking trade execution:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<any>> AnalyticsService::addTradePerformance(std::shared_ptr<TradePerformanceData> data, boolean isSimulation)
{
    try
    {
        auto id = as<std::shared_ptr<>>(uuidv4());
        string_empty + string + std:("-") + string + std:("-") + string + std:("-") + string + std:("-") + string + string_empty;
        auto tradeData = utils::assign(object{
            object::pair{std:("id"), std:("id")}, 
            object::pair{std:("isSimulation"), std:("isSimulation")}, 
            object::pair{std:("created_at"), ((std::make_shared<Date>()))->toISOString()}
        }, data);
        std::async([=]() { this->runtime->setCache(std:("trade_performance:") + data->token_address + std:(":") + data->buy_timeStamp + string_empty, tradeData); });
        auto allTradesKey = (isSimulation) ? std:("all_simulation_trades") : std:("all_trades");
        auto allTrades = OR(((std::async([=]() { this->runtime->getCache<array<string>>(allTradesKey); }))), (array<any>()));
        allTrades["push"](string_empty + data->token_address + std:(":") + data->buy_timeStamp + string_empty);
        std::async([=]() { this->runtime->setCache(allTradesKey, allTrades); });
        std::async([=]() { this->updateTokenStatistics(data->token_address, object{
            object::pair{std:("profit_usd"), data->profit_usd}, 
            object::pair{std:("profit_percent"), data->profit_percent}, 
            object::pair{std:("rapidDump"), data->rapidDump}
        }); });
        return tradeData;
    }
    catch (const any& error)
    {
        console->log(std:("Error adding trade performance:"), error);
        throw any(error);
    }
}

std::shared_ptr<Promise<void>> AnalyticsService::updateTokenStatistics(string tokenAddress, object data)
{
    try
    {
        auto stats = OR(((std::async([=]() { this->runtime->getCache<any>(std:("token_stats:") + tokenAddress + string_empty); }))), (object{
            object::pair{std:("trades"), 0}, 
            object::pair{std:("total_profit_usd"), 0}, 
            object::pair{std:("average_profit_percent"), 0}, 
            object::pair{std:("rapid_dumps"), 0}
        }));
        stats["trades"] += 1;
        stats["total_profit_usd"] += data["profit_usd"];
        stats["average_profit_percent"] = (stats["average_profit_percent"] * (stats["trades"] - 1) + data["profit_percent"]) / stats["trades"];
        if (data["rapidDump"]) stats["rapid_dumps"] += 1;
        std::async([=]() { this->runtime->setCache(std:("token_stats:") + tokenAddress + string_empty, stats); });
    }
    catch (const any& error)
    {
        console->log(std:("Error updating token statistics:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

