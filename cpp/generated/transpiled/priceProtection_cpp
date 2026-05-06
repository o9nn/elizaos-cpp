#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/services/priceProtection.h"

PriceProtectionService::PriceProtectionService() {
    this->marketDataService = std::make_shared<MarketDataService>();
}

std::shared_ptr<Promise<std::shared_ptr<ValidationResult>>> PriceProtectionService::validateQuotePrice(string tokenId, string tokenAddress, std::shared_ptr<Chain> chain, double priceAtQuote, double maxDeviationBps)
{
    auto marketData = std::async([=]() { MarketDataDB::getMarketData(tokenId); });
    double currentPrice;
    if (OR((!marketData), (Date->now() - marketData["lastUpdated"] > 300000))) {
        currentPrice = std::async([=]() { this->marketDataService->fetchTokenPrice(tokenAddress, chain); });
    } else {
        currentPrice = marketData["priceUsd"];
    }
    auto deviation = Math->abs(currentPrice - priceAtQuote);
    auto deviationBps = Math->floor((deviation / priceAtQuote) * 10000);
    auto isValid = deviationBps <= maxDeviationBps;
    return object{
        object::pair{std::string("isValid"), std::string("isValid")}, 
        object::pair{std::string("currentPrice"), std::string("currentPrice")}, 
        object::pair{std::string("priceAtQuote"), std::string("priceAtQuote")}, 
        object::pair{std::string("deviation"), std::string("deviation")}, 
        object::pair{std::string("deviationBps"), std::string("deviationBps")}, 
        object::pair{std::string("maxAllowedDeviationBps"), maxDeviationBps}, 
        object::pair{std::string("reason"), (isValid) ? any(undefined) : any(std::string("Price moved ") + (deviationBps / 100) + std::string("%, exceeding maximum allowed ") + (maxDeviationBps / 100) + std::string("%"))}
    };
}

