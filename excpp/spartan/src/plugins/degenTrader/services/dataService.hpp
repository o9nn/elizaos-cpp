#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/trading.hpp"
#include ".utils/cacheManager.hpp"
#include ".utils/wallet.hpp"
#include "analyticsService.hpp"
#include "calculation/birdeye.hpp"
#include "calculation/scoring.hpp"
#include "calculation/technicalAnalysis.hpp"
#include "calculation/tradeCalculation.hpp"
#include "elizaos/core.hpp"
#include "validation/tokenSecurity.hpp"
#include "walletService.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class DataService {
public:
    DataService();
    std::future<void> initialize();
    std::future<void> stop();
    std::future<std::vector<TokenSignal>> getBirdeyeSignals();
    std::future<std::vector<TokenSignal>> getTwitterSignals();
    std::future<std::vector<TokenSignal>> getCMCSignals();
    Promise< getTokenMarketData(const std::string& tokenAddress);
    std::future<std::any> getTokensMarketData(const std::vector<std::string>& tokenAddresses);
    std::future<std::vector<std::string>> getMonitoredTokens();
    std::future<std::vector<std::any>> getPositions();
    void getDefaultRecommendation();

private:
    CacheManager cacheManager_;
    BirdeyeService birdeyeService_;
    AnalyticsService analyticsService_;
    TechnicalAnalysisService technicalAnalysisService_;
    ScoringService scoringService_;
    TokenSecurityService tokenSecurityService_;
    TradeCalculationService tradeCalculationService_;
};


} // namespace elizaos
