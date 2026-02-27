#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/services/dataService.h"

DataService::DataService(std::shared_ptr<IAgentRuntime> runtime_, std::shared_ptr<WalletService> walletService_) : runtime(runtime_), walletService(walletService_)  {
    std::shared_ptr<DataService> _this(this, [] (auto&) {/*to be finished*/});
    this->cacheManager = std::make_shared<CacheManager>();
    this->analyticsService = std::make_shared<AnalyticsService>(runtime);
    this->technicalAnalysisService = std::make_shared<TechnicalAnalysisService>(runtime, walletService, _this, this->analyticsService);
    this->scoringService = std::make_shared<ScoringService>(runtime, walletService, _this, this->analyticsService);
    this->tokenSecurityService = std::make_shared<TokenSecurityService>(runtime, walletService, _this, this->analyticsService);
    this->tradeCalculationService = std::make_shared<TradeCalculationService>(runtime, walletService, _this, this->analyticsService);
}

std::shared_ptr<Promise<void>> DataService::initialize()
{
    logger->info(std:("Initializing data service"));
    auto apiKey = process->env->BIRDEYE_API_KEY;
    if (!apiKey) {
        throw any(std::make_shared<Error>(std:("Birdeye API key not found")));
    }
    this->birdeyeService = std::make_shared<BirdeyeService>(apiKey);
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<void>> DataService::stop()
{
    std::async([=]() { this->cacheManager->clear(); });
    return std::shared_ptr<Promise<void>>();
}

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> DataService::getBirdeyeSignals()
{
    try
    {
        auto trendingTokens = OR(((std::async([=]() { this->cacheManager->get<array<any>>(std:("birdeye_trending_tokens")); }))), (array<any>()));
        return Promise->all(trendingTokens->map([=](auto token) mutable
        {
            auto marketData = std::async([=]() { this->getTokenMarketData(token["address"]); });
            auto technicalSignals = std::async([=]() { this->technicalAnalysisService->calculateTechnicalSignals(marketData); });
            return object{
                object::pair{std:("address"), token["address"]}, 
                object::pair{std:("symbol"), token["symbol"]}, 
                object::pair{std:("marketCap"), marketData["marketCap"]}, 
                object::pair{std:("volume24h"), marketData["volume24h"]}, 
                object::pair{std:("price"), marketData["price"]}, 
                object::pair{std:("liquidity"), marketData["liquidity"]}, 
                object::pair{std:("score"), 0}, 
                object::pair{std:("reasons"), array<string>{ std:("Trending on Birdeye with ") + marketData["volume24h"] + std:("$ 24h volume") }}, 
                object::pair{std:("technicalSignals"), utils::assign(object{
                    , 
                    object::pair{std:("macd"), object{
                        object::pair{std:("value"), technicalSignals["macd"]["macd"]}, 
                        object::pair{std:("signal"), technicalSignals["macd"]["signal"]}, 
                        object::pair{std:("histogram"), technicalSignals["macd"]["histogram"]}
                    }}
                }, technicalSignals)}
            };
        }
        ));
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting Birdeye signals:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> DataService::getTwitterSignals()
{
    try
    {
        auto twitterSignals = OR(((std::async([=]() { this->cacheManager->get<array<any>>(std:("twitter_parsed_signals")); }))), (array<any>()));
        return twitterSignals->map([=](auto signal) mutable
        {
            return (object{
                object::pair{std:("address"), signal["tokenAddress"]}, 
                object::pair{std:("symbol"), signal["symbol"]}, 
                object::pair{std:("marketCap"), signal["marketCap"]}, 
                object::pair{std:("volume24h"), signal["volume24h"]}, 
                object::pair{std:("price"), signal["price"]}, 
                object::pair{std:("liquidity"), signal["liquidity"]}, 
                object::pair{std:("score"), 0}, 
                object::pair{std:("reasons"), array<string>{ std:("High social activity: ") + signal["mentionCount"] + std:(" mentions") }}, 
                object::pair{std:("socialMetrics"), object{
                    object::pair{std:("mentionCount"), signal["mentionCount"]}, 
                    object::pair{std:("sentiment"), signal["sentiment"]}, 
                    object::pair{std:("influencerMentions"), signal["influencerMentions"]}
                }}
            });
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting Twitter signals:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> DataService::getCMCSignals()
{
    try
    {
        auto cmcTokens = OR(((std::async([=]() { this->cacheManager->get<array<any>>(std:("cmc_trending_tokens")); }))), (array<any>()));
        return cmcTokens->map([=](auto token) mutable
        {
            return (object{
                object::pair{std:("address"), token["address"]}, 
                object::pair{std:("symbol"), token["symbol"]}, 
                object::pair{std:("marketCap"), token["marketCap"]}, 
                object::pair{std:("volume24h"), token["volume24h"]}, 
                object::pair{std:("price"), token["price"]}, 
                object::pair{std:("liquidity"), token["liquidity"]}, 
                object::pair{std:("score"), 0}, 
                object::pair{std:("reasons"), array<string>{ std:("Trending on CMC: ") + token["cmcRank"] + std:(" rank") }}, 
                object::pair{std:("cmcMetrics"), object{
                    object::pair{std:("rank"), token["cmcRank"]}, 
                    object::pair{std:("priceChange24h"), token["priceChange24h"]}, 
                    object::pair{std:("volumeChange24h"), token["volumeChange24h"]}
                }}
            });
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting CMC signals:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<object>> DataService::getTokenMarketData(string tokenAddress)
{
    auto cacheKey = std:("market_data_") + tokenAddress + string_empty;
    auto cached = std::async([=]() { this->cacheManager->get<any>(cacheKey); });
    if (cached) return cached;
    auto result = std::async([=]() { this->birdeyeService->getTokenMarketData(tokenAddress); });
    std::async([=]() { this->cacheManager->set(cacheKey, result, 10 * 60 * 1000); });
    return utils::assign(object{
        , 
        object::pair{std:("volumeHistory"), array<any>()}
    }, result);
}

std::shared_ptr<Promise<any>> DataService::getTokensMarketData(array<string> tokenAddresses)
{
    auto missing = array<string>();
    auto tokenDb = object{};
    for (auto& ca : tokenAddresses)
    {
        auto cached = std::async([=]() { this->cacheManager->get<any>(std:("market_data_") + ca + string_empty); });
        if (!cached) {
            missing->push(ca);
        } else {
            tokenDb[ca] = cached;
        }
    }
    if (missing->get_length()) {
        auto newData = std::async([=]() { this->birdeyeService->getTokensMarketData(missing); });
        for (auto& [address, data] : Object->entries(newData))
        {
            auto cacheKey = std:("market_data_") + address + string_empty;
            std::async([=]() { this->cacheManager->set(cacheKey, data, 10 * 60 * 1000); });
            tokenDb[address] = data;
        }
    }
    return tokenDb;
}

std::shared_ptr<Promise<array<string>>> DataService::getMonitoredTokens()
{
    try
    {
        auto tasks = std::async([=]() { this->runtime->getTasks(object{
            object::pair{std:("tags"), array<string>{ std:("degen_trader"), std:("EXECUTE_SELL") }}
        }); });
        shared tokenAddresses = std::make_shared<Set<string>>();
        tasks->forEach([=](auto task) mutable
        {
            auto metadata = as<object>(task["metadata"]);
            if (metadata["signal"]["tokenAddress"]) {
                tokenAddresses->add(metadata["signal"]["tokenAddress"]);
            }
        }
        );
        return Array->from(tokenAddresses);
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting monitored tokens:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<any>>> DataService::getPositions()
{
    try
    {
        auto monitoredTokens = std::async([=]() { this->getMonitoredTokens(); });
        if (!monitoredTokens->get_length()) {
            return array<any>();
        }
        auto positions = std::async([=]() { Promise->all(monitoredTokens->map([=](auto tokenAddress) mutable
        {
            try
            {
                auto balance = std::async([=]() { getTokenBalance(this->runtime, tokenAddress); });
                auto marketData = std::async([=]() { this->getTokenMarketData(tokenAddress); });
                return object{
                    object::pair{std:("tokenAddress"), std:("tokenAddress")}, 
                    object::pair{std:("balance"), std:("balance")}, 
                    object::pair{std:("currentPrice"), marketData["price"]}, 
                    object::pair{std:("value"), Number(balance->balance) * marketData["price"]}, 
                    object::pair{std:("lastUpdated"), ((std::make_shared<Date>()))->toISOString()}
                };
            }
            catch (const any& error)
            {
                logger->error(std:("Error getting position for token ") + tokenAddress + std:(":"), error);
                return nullptr;
            }
        }
        )); });
        return positions->filter([=](auto position) mutable
        {
            return position != nullptr;
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std:("Error getting positions:"), error);
        return array<any>();
    }
}

any DataService::getDefaultRecommendation()
{
    return object{
        object::pair{std:("recommended_buy"), std:("SOL")}, 
        object::pair{std:("recommend_buy_address"), std:("So11111111111111111111111111111111111111112")}, 
        object::pair{std:("reason"), std:("Default recommendation")}, 
        object::pair{std:("marketcap"), 0}, 
        object::pair{std:("buy_amount"), 0.1}
    };
}

