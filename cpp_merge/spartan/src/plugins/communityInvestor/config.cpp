#include "config.hpp"
#include <string>

double getConvictionMultiplier(Conviction conviction)
{
    switch (conviction)
    {
    case Conviction::NONE:
        return 0;
    case Conviction::LOW:
        return 0.5;
    case Conviction::MEDIUM:
        return 1;
    case Conviction::HIGH:
        return 1.5;
    case Conviction::VERY_HIGH:
        return 2;
    default:
        return 1;
    }
};


double getLiquidityMultiplier(double liquidity)
{
    if (liquidity < 10000) return 0.5;
    if (liquidity < 50000) return 0.75;
    if (liquidity < 100000) return 1;
    if (liquidity < 500000) return 1.25;
    return 1.5;
};


double getMarketCapMultiplier(double marketCap)
{
    if (marketCap < 100000) return 1.5;
    if (marketCap < 1000000) return 1.25;
    if (marketCap < 10000000) return 1;
    if (marketCap < 50000000) return 0.75;
    return 0.5;
};


double getVolumeMultiplier(double volume)
{
    if (volume < 10000) return 0.5;
    if (volume < 50000) return 0.75;
    if (volume < 100000) return 1;
    if (volume < 500000) return 1.25;
    return 1.5;
};


std::shared_ptr<TradingConfig> DEFAULT_TRADING_CONFIG = object{
    object::pair{std::string("slippageBps"), 100}, 
    object::pair{std::string("forceSimulation"), false}, 
    object::pair{std::string("defaultChain"), std::string("solana")}, 
    object::pair{std::string("maxPositionsPerToken"), 3}, 
    object::pair{std::string("maxPositionsPerRecommender"), 5}, 
    object::pair{std::string("minLiquidityUsd"), 10000}, 
    object::pair{std::string("maxMarketCapUsd"), 100000000}, 
    object::pair{std::string("buyAmountConfig"), object{
        object::pair{std::string("baseAmount"), 0.1}, 
        object::pair{std::string("minAmount"), 0.01}, 
        object::pair{std::string("maxAmount"), 1}, 
        object::pair{std::string("trustScoreMultiplier"), 0.5}, 
        object::pair{std::string("convictionMultiplier"), 0.3}
    }}
};
std::shared_ptr<DatabaseConfig> DEFAULT_DATABASE_CONFIG = object{
    object::pair{std::string("schemaVersion"), std::string("1.0")}, 
    object::pair{std::string("enableCaching"), true}, 
    object::pair{std::string("cacheTimeout"), 3600}
};
std::shared_ptr<MemoryConfig> DEFAULT_MEMORY_CONFIG = object{
    object::pair{std::string("embeddingModel"), std::string("text-embedding-ada-002")}, 
    object::pair{std::string("embeddingDimension"), 1536}, 
    object::pair{std::string("similarityThreshold"), 0.7}, 
    object::pair{std::string("cacheTimeout"), 3600}
};

void Main(void)
{
}

MAIN
