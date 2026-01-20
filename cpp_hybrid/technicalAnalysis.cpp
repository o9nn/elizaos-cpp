#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/calculation/technicalAnalysis.h"

any TechnicalAnalysisService::calculateTechnicalSignals(any marketData)
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

TechnicalAnalysisService::TechnicalAnalysisService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

