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
    logger->info(std::string("Initializing data service"));
    auto apiKey = process->env->BIRDEYE_API_KEY;
    if (!apiKey) {
        throw any(std::make_shared<Error>(std::string("Birdeye API key not found")));
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
        auto trendingTokens = OR(((std::async([=]() { this->cacheManager->get<array<any>>(std::string("birdeye_trending_tokens")); }))), (array<any>()));
        return Promise->all(trendingTokens->map([=](auto token) mutable
        {
            auto marketData = std::async([=]() { this->getTokenMarketData(token["address"]); });
            auto technicalSignals = std::async([=]() { this->technicalAnalysisService->calculateTechnicalSignals(marketData); });
            return object{
                object::pair{std::string("address"), token["address"]}, 
                object::pair{std::string("symbol"), token["symbol"]}, 
                object::pair{std::string("marketCap"), marketData["marketCap"]}, 
                object::pair{std::string("volume24h"), marketData["volume24h"]}, 
                object::pair{std::string("price"), marketData["price"]}, 
                object::pair{std::string("liquidity"), marketData["liquidity"]}, 
                object::pair{std::string("score"), 0}, 
                object::pair{std::string("reasons"), array<string>{ std::string("Trending on Birdeye with ") + marketData["volume24h"] + std::string("$ 24h volume") }}, 
                object::pair{std::string("technicalSignals"), utils::assign(object{
                    , 
                    object::pair{std::string("macd"), object{
                        object::pair{std::string("value"), technicalSignals["macd"]["macd"]}, 
                        object::pair{std::string("signal"), technicalSignals["macd"]["signal"]}, 
                        object::pair{std::string("histogram"), technicalSignals["macd"]["histogram"]}
                    }}
                }, technicalSignals)}
            };
        }
        ));
    }
    catch (const any& error)
    {
        logger->error(std::string("Error getting Birdeye signals:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> DataService::getTwitterSignals()
{
    try
    {
        auto twitterSignals = OR(((std::async([=]() { this->cacheManager->get<array<any>>(std::string("twitter_parsed_signals")); }))), (array<any>()));
        return twitterSignals->map([=](auto signal) mutable
        {
            return (object{
                object::pair{std::string("address"), signal["tokenAddress"]}, 
                object::pair{std::string("symbol"), signal["symbol"]}, 
                object::pair{std::string("marketCap"), signal["marketCap"]}, 
                object::pair{std::string("volume24h"), signal["volume24h"]}, 
                object::pair{std::string("price"), signal["price"]}, 
                object::pair{std::string("liquidity"), signal["liquidity"]}, 
                object::pair{std::string("score"), 0}, 
                object::pair{std::string("reasons"), array<string>{ std::string("High social activity: ") + signal["mentionCount"] + std::string(" mentions") }}, 
                object::pair{std::string("socialMetrics"), object{
                    object::pair{std::string("mentionCount"), signal["mentionCount"]}, 
                    object::pair{std::string("sentiment"), signal["sentiment"]}, 
                    object::pair{std::string("influencerMentions"), signal["influencerMentions"]}
                }}
            });
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std::string("Error getting Twitter signals:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<array<std::shared_ptr<TokenSignal>>>> DataService::getCMCSignals()
{
    try
    {
        auto cmcTokens = OR(((std::async([=]() { this->cacheManager->get<array<any>>(std::string("cmc_trending_tokens")); }))), (array<any>()));
        return cmcTokens->map([=](auto token) mutable
        {
            return (object{
                object::pair{std::string("address"), token["address"]}, 
                object::pair{std::string("symbol"), token["symbol"]}, 
                object::pair{std::string("marketCap"), token["marketCap"]}, 
                object::pair{std::string("volume24h"), token["volume24h"]}, 
                object::pair{std::string("price"), token["price"]}, 
                object::pair{std::string("liquidity"), token["liquidity"]}, 
                object::pair{std::string("score"), 0}, 
                object::pair{std::string("reasons"), array<string>{ std::string("Trending on CMC: ") + token["cmcRank"] + std::string(" rank") }}, 
                object::pair{std::string("cmcMetrics"), object{
                    object::pair{std::string("rank"), token["cmcRank"]}, 
                    object::pair{std::string("priceChange24h"), token["priceChange24h"]}, 
                    object::pair{std::string("volumeChange24h"), token["volumeChange24h"]}
                }}
            });
        }
        );
    }
    catch (const any& error)
    {
        logger->error(std::string("Error getting CMC signals:"), error);
        return array<any>();
    }
}

std::shared_ptr<Promise<object>> DataService::getTokenMarketData(string tokenAddress)
{
    auto cacheKey = std::string("market_data_") + tokenAddress + string_empty;
    auto cached = std::async([=]() { this->cacheManager->get<any>(cacheKey); });
    if (cached) return cached;
    auto result = std::async([=]() { this->birdeyeService->getTokenMarketData(tokenAddress); });
    std::async([=]() { this->cacheManager->set(cacheKey, result, 10 * 60 * 1000); });
    return utils::assign(object{
        , 
        object::pair{std::string("volumeHistory"), array<any>()}
    }, result);
}

std::shared_ptr<Promise<any>> DataService::getTokensMarketData(array<string> tokenAddresses)
{
    auto missing = array<string>();
    auto tokenDb = object{};
    for (auto& ca : tokenAddresses)
    {
        auto cached = std::async([=]() { this->cacheManager->get<any>(std::string("market_data_") + ca + string_empty); });
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
            auto cacheKey = std::string("market_data_") + address + string_empty;
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
            object::pair{std::string("tags"), array<string>{ std::string("degen_trader"), std::string("EXECUTE_SELL") }}
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
        logger->error(std::string("Error getting monitored tokens:"), error);
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
                    object::pair{std::string("tokenAddress"), std::string("tokenAddress")}, 
                    object::pair{std::string("balance"), std::string("balance")}, 
                    object::pair{std::string("currentPrice"), marketData["price"]}, 
                    object::pair{std::string("value"), Number(balance->balance) * marketData["price"]}, 
                    object::pair{std::string("lastUpdated"), ((std::make_shared<Date>()))->toISOString()}
                };
            }
            catch (const any& error)
            {
                logger->error(std::string("Error getting position for token ") + tokenAddress + std::string(":"), error);
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
        logger->error(std::string("Error getting positions:"), error);
        return array<any>();
    }
}

any DataService::getDefaultRecommendation()
{
    return object{
        object::pair{std::string("recommended_buy"), std::string("SOL")}, 
        object::pair{std::string("recommend_buy_address"), std::string("So11111111111111111111111111111111111111112")}, 
        object::pair{std::string("reason"), std::string("Default recommendation")}, 
        object::pair{std::string("marketcap"), 0}, 
        object::pair{std::string("buy_amount"), 0.1}
    };
}

