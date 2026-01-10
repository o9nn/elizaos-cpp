#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/calculation/scoring.h"

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> ScoringService::scoreTokenSignals(array<std::shared_ptr<TokenSignal>> signals)
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

ScoringService::ScoringService(std::shared_ptr<IAgentRuntime> runtime, std::shared_ptr<WalletService> walletService, std::shared_ptr<DataService> dataService, std::shared_ptr<AnalyticsService> analyticsService) : BaseTradeService(runtime, walletService, dataService, analyticsService) {
}

