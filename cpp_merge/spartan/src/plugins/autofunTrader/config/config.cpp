#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofunTrader/config/config.h"

object PROVIDER_CONFIG = object{
    object::pair{std::string("BIRDEYE_API"), std::string("https://public-api.birdeye.so")}, 
    object::pair{std::string("TOKEN_SECURITY_ENDPOINT"), std::string("/defi/token_security?address=")}, 
    object::pair{std::string("TOKEN_METADATA_ENDPOINT"), std::string("/defi/v3/token/meta-data/single?address=")}, 
    object::pair{std::string("MARKET_SEARCH_ENDPOINT"), std::string("/defi/v3/token/trade-data/single?address=")}, 
    object::pair{std::string("TOKEN_PRICE_CHANGE_ENDPOINT"), std::string("/defi/v3/search?chain=solana&target=token&sort_by=price_change_24h_percent&sort_type=desc&verify_token=true&markets=Raydium&limit=20")}, 
    object::pair{std::string("TOKEN_VOLUME_24_CHANGE_ENDPOINT"), std::string("/defi/v3/search?chain=solana&target=token&sort_by=volume_24h_change_percent&sort_type=desc&verify_token=true&markets=Raydium&limit=20")}, 
    object::pair{std::string("TOKEN_BUY_24_CHANGE_ENDPOINT"), std::string("/defi/v3/search?chain=solana&target=token&sort_by=buy_24h_change_percent&sort_type=desc&verify_token=true&markets=Raydium&offset=0&limit=20")}, 
    object::pair{std::string("TOKEN_SECURITY_ENDPOINT_BASE"), std::string("/defi/token_security?address=")}, 
    object::pair{std::string("TOKEN_METADATA_ENDPOINT_BASE"), std::string("/defi/v3/token/meta-data/single?address=")}, 
    object::pair{std::string("MARKET_SEARCH_ENDPOINT_BASE"), std::string("/defi/v3/token/trade-data/single?address=")}, 
    object::pair{std::string("TOKEN_PRICE_CHANGE_ENDPOINT_BASE"), std::string("/defi/v3/search?chain=base&target=token&sort_by=price_change_24h_percent&sort_type=desc&offset=0&limit=20")}, 
    object::pair{std::string("TOKEN_VOLUME_24_ENDPOINT_BASE"), std::string("/defi/v3/search?chain=base&target=token&sort_by=volume_24h_usd&sort_type=desc&offset=2&limit=20")}, 
    object::pair{std::string("TOKEN_BUY_24_ENDPOINT_BASE"), std::string("/defi/v3/search?chain=base&target=token&sort_by=buy_24h&sort_type=desc&offset=2&limit=20")}, 
    object::pair{std::string("MAX_RETRIES"), 3}, 
    object::pair{std::string("RETRY_DELAY"), 2000}
};
object CHAIN_CONFIG = object{
    object::pair{std::string("SOLANA_ENABLED"), false}, 
    object::pair{std::string("BASE_ENABLED"), true}
};
object BASE_CONFIG = object{
    object::pair{std::string("RPC_URL"), OR((process->env->EVM_PROVIDER_URL), (std::string("https://mainnet.base.org")))}, 
    object::pair{std::string("ROUTER_ADDRESS"), std::string("0x327Df1E6de05895d2ab08513aaDD9313Fe505d86")}, 
    object::pair{std::string("WETH_ADDRESS"), std::string("0x4200000000000000000000000000000000000006")}, 
    object::pair{std::string("CHAIN_ID"), 8453}, 
    object::pair{std::string("AERODROME"), object{
        object::pair{std::string("WETH"), std::string("0x4200000000000000000000000000000000000006")}, 
        object::pair{std::string("USDC"), std::string("0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913")}, 
        object::pair{std::string("USDT"), std::string("0x50c5725949A6F0c72E6C4a641F24049A917DB0Cb")}
    }}
};
object ZEROEX_CONFIG = object{
    object::pair{std::string("API_URL"), std::string("https://api.0x.org")}, 
    object::pair{std::string("API_KEY"), OR((process->env->ZEROEX_API_KEY), (string_empty))}, 
    object::pair{std::string("QUOTE_ENDPOINT"), std::string("/swap/permit2/quote")}, 
    object::pair{std::string("PRICE_ENDPOINT"), std::string("/swap/permit2/price")}, 
    object::pair{std::string("SUPPORTED_CHAINS"), object{
        object::pair{std::string("BASE"), 8453}
    }}, 
    object::pair{std::string("HEADERS"), object{
        object::pair{std::string("Content-Type"), std::string("application/json")}, 
        object::pair{std::string("0x-api-key"), OR((process->env->ZEROEX_API_KEY), (string_empty))}, 
        object::pair{std::string("0x-version"), std::string("v2")}
    }}
};
object REQUIRED_SETTINGS = object{
    object::pair{std::string("SOLANA_RPC_URL"), std::string("Solana RPC endpoint URL")}, 
    object::pair{std::string("SOLANA_PRIVATE_KEY"), std::string("Private key for trading wallet")}, 
    object::pair{std::string("BIRDEYE_API_KEY"), std::string("API key for Birdeye data")}, 
    object::pair{std::string("TRADER_SELL_KUMA"), std::string("Webhook URL for sell notifications")}
};
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
