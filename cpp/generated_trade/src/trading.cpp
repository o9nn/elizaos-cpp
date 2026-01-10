#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/config/trading.h"

object DEFAULT_CONFIG = object{
    object::pair{std::string("intervals"), object{
        object::pair{std::string("priceCheck"), 60000}, 
        object::pair{std::string("walletSync"), 600000}, 
        object::pair{std::string("performanceMonitor"), 3600000}
    }}, 
    object::pair{std::string("thresholds"), object{
        object::pair{std::string("minLiquidity"), 50000}, 
        object::pair{std::string("minVolume"), 100000}, 
        object::pair{std::string("minScore"), 60}
    }}, 
    object::pair{std::string("riskLimits"), object{
        object::pair{std::string("maxPositionSize"), 0.2}, 
        object::pair{std::string("maxDrawdown"), 0.1}, 
        object::pair{std::string("stopLossPercentage"), 0.05}, 
        object::pair{std::string("takeProfitPercentage"), 0.2}
    }}, 
    object::pair{std::string("slippageSettings"), object{
        object::pair{std::string("baseSlippage"), 0.5}, 
        object::pair{std::string("maxSlippage"), 1}, 
        object::pair{std::string("liquidityMultiplier"), 1}, 
        object::pair{std::string("volumeMultiplier"), 1}
    }}
};
object SAFETY_LIMITS = object{
    object::pair{std::string("MINIMUM_TRADE"), 0.1}, 
    object::pair{std::string("MAX_SLIPPAGE"), 0.05}, 
    object::pair{std::string("MIN_LIQUIDITY"), 50000}, 
    object::pair{std::string("MIN_VOLUME"), 10000}, 
    object::pair{std::string("MAX_PRICE_CHANGE"), 30}
};

void Main(void)
{
}

MAIN
