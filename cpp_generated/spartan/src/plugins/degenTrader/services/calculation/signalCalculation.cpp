#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/calculation/signalCalculation.h"

any SignalCalculationService::calculateTechnicalSignals(any marketData)
{
    auto rsi = this->analyticsService->calculateRSI(marketData["priceHistory"], 14);
    auto macd = this->analyticsService->calculateMACD(marketData["priceHistory"]);
    auto volatility = (marketData["priceHistory"]["length"] > 1) ? any(Math->abs(const_(marketData["priceHistory"])[marketData["priceHistory"]["length"] - 1] - const_(marketData["priceHistory"])[marketData["priceHistory"]["length"] - 2]) / const_(marketData["priceHistory"])[marketData["priceHistory"]["length"] - 2]) : any(0);
    auto volumeTrend = (marketData["volume24h"] > marketData["marketCap"] * 0.1) ? std::string("increasing") : std::string("stable");
    auto unusualActivity = marketData["volume24h"] > marketData["marketCap"] * 0.2;
    return object{
        object::pair{std::string("rsi"), std::string("rsi")}, 
        object::pair{std::string("macd"), std::string("macd")}, 
        object::pair{std::string("volumeProfile"), object{
            object::pair{std::string("trend"), as<any>(volumeTrend)}, 
            object::pair{std::string("unusualActivity"), std::string("unusualActivity")}
        }}, 
        object::pair{std::string("volatility"), std::string("volatility")}
    };
}

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> SignalCalculationService::scoreTokenSignals(array<std::shared_ptr<TokenSignal>> signals)
{
    auto tokenMap = std::make_shared<Map<string, std::shared_ptr<TokenSignal>>>();
    for (auto& signal : signals)
    {
        if (tokenMap->has(signal->address)) {
            auto existing = tokenMap->get(signal->address);
            existing->reasons->push(const_(signal->reasons)[0]);
            existing->score += signal->score;
        } else {
            tokenMap->set(signal->address, signal);
        }
    }
    auto scoredTokens = std::async([=]() { Promise->all(Array->from(tokenMap->values())->map([=](auto token) mutable
    {
        auto score = 0;
        if (token->technicalSignals) {
            score += std::async([=]() { this->analyticsService->scoreTechnicalSignals(token->technicalSignals); });
        }
        if (token->socialMetrics) {
            score += std::async([=]() { this->analyticsService->scoreSocialMetrics(token->socialMetrics); });
        }
        score += std::async([=]() { this->analyticsService->scoreMarketMetrics(object{
            object::pair{std::string("marketCap"), token->marketCap}, 
            object::pair{std::string("volume24h"), token->volume24h}, 
            object::pair{std::string("liquidity"), token->liquidity}
        }); });
        token->score = score;
        return token;
    }
    )); });
    return scoredTokens->filter([=](auto token) mutable
    {
        return AND((AND((token->score >= 60), (token->liquidity >= 50000))), (token->volume24h >= 100000));
    }
    )->sort([=](auto a, auto b) mutable
    {
        return b->score - a->score;
    }
    );
}

std::shared_ptr<Promise<double>> SignalCalculationService::calculateDrawdown(object portfolio)
{
    try
    {
        auto highWaterMark = std::async([=]() { this->getHighWaterMark(); });
        auto currentDrawdown = (highWaterMark > 0) ? any((highWaterMark - portfolio["totalValue"]) / highWaterMark) : any(0);
        if (portfolio["totalValue"] > highWaterMark) {
            std::async([=]() { this->updateHighWaterMark(portfolio["totalValue"]); });
        }
        return Math->max(0, currentDrawdown);
    }
    catch (const any& error)
    {
        logger->error(std::string("Error calculating drawdown:"), error);
        return 0;
    }
}

std::shared_ptr<Promise<double>> SignalCalculationService::getHighWaterMark()
{
    try
    {
        auto stored = std::async([=]() { this->runtime->databaseAdapter->getValue(std::string("high_water_mark")); });
        return (stored) ? any(Number(stored)) : any(0);
    }
    catch (const any& error)
    {
        logger->error(std::string("Error getting high water mark:"), error);
        return 0;
    }
}

std::shared_ptr<Promise<void>> SignalCalculationService::updateHighWaterMark(double value)
{
    try
    {
        std::async([=]() { this->runtime->databaseAdapter->setValue(std::string("high_water_mark"), value->toString()); });
    }
    catch (const any& error)
    {
        logger->error(std::string("Error updating high water mark:"), error);
    }
    return std::shared_ptr<Promise<void>>();
}

SignalCalculationService::SignalCalculationService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

