#include "priceProtection.hpp"

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
        object::pair{std:("isValid"), std:("isValid")}, 
        object::pair{std:("currentPrice"), std:("currentPrice")}, 
        object::pair{std:("priceAtQuote"), std:("priceAtQuote")}, 
        object::pair{std:("deviation"), std:("deviation")}, 
        object::pair{std:("deviationBps"), std:("deviationBps")}, 
        object::pair{std:("maxAllowedDeviationBps"), maxDeviationBps}, 
        object::pair{std:("reason"), (isValid) ? any(undefined) (std:("Price moved ") + (deviationBps / 100) + std:("%, exceeding maximum allowed ") + (maxDeviationBps / 100) + std:("%"))}
    };
}

