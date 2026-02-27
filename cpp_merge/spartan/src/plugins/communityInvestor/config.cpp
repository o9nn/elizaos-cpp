#include "config.hpp"

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
    object::pair{std:("slippageBps"), 100}, 
    object::pair{std:("forceSimulation"), false}, 
    object::pair{std:("defaultChain"), std:("solana")}, 
    object::pair{std:("maxPositionsPerToken"), 3}, 
    object::pair{std:("maxPositionsPerRecommender"), 5}, 
    object::pair{std:("minLiquidityUsd"), 10000}, 
    object::pair{std:("maxMarketCapUsd"), 100000000}, 
    object::pair{std:("buyAmountConfig"), object{
        object::pair{std:("baseAmount"), 0.1}, 
        object::pair{std:("minAmount"), 0.01}, 
        object::pair{std:("maxAmount"), 1}, 
        object::pair{std:("trustScoreMultiplier"), 0.5}, 
        object::pair{std:("convictionMultiplier"), 0.3}
    }}
};
std::shared_ptr<DatabaseConfig> DEFAULT_DATABASE_CONFIG = object{
    object::pair{std:("schemaVersion"), std:("1.0")}, 
    object::pair{std:("enableCaching"), true}, 
    object::pair{std:("cacheTimeout"), 3600}
};
std::shared_ptr<MemoryConfig> DEFAULT_MEMORY_CONFIG = object{
    object::pair{std:("embeddingModel"), std:("text-embedding-ada-002")}, 
    object::pair{std:("embeddingDimension"), 1536}, 
    object::pair{std:("similarityThreshold"), 0.7}, 
    object::pair{std:("cacheTimeout"), 3600}
};

void Main(void)
{
}

MAIN
