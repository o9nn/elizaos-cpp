#include "config.hpp"

object PROVIDER_CONFIG = object{
    object::pair{std:("BIRDEYE_API"), std:("https://public-api.birdeye.so")}, 
    object::pair{std:("TOKEN_SECURITY_ENDPOINT"), std:("/defi/token_security?address=")}, 
    object::pair{std:("TOKEN_METADATA_ENDPOINT"), std:("/defi/v3/token/meta-data/single?address=")}, 
    object::pair{std:("MARKET_SEARCH_ENDPOINT"), std:("/defi/v3/token/trade-data/single?address=")}, 
    object::pair{std:("TOKEN_PRICE_CHANGE_ENDPOINT"), std:("/defi/v3/search?chain=solana&target=token&sort_by=price_change_24h_percent&sort_type=desc&verify_token=true&markets=Raydium&limit=20")}, 
    object::pair{std:("TOKEN_VOLUME_24_CHANGE_ENDPOINT"), std:("/defi/v3/search?chain=solana&target=token&sort_by=volume_24h_change_percent&sort_type=desc&verify_token=true&markets=Raydium&limit=20")}, 
    object::pair{std:("TOKEN_BUY_24_CHANGE_ENDPOINT"), std:("/defi/v3/search?chain=solana&target=token&sort_by=buy_24h_change_percent&sort_type=desc&verify_token=true&markets=Raydium&offset=0&limit=20")}, 
    object::pair{std:("TOKEN_SECURITY_ENDPOINT_BASE"), std:("/defi/token_security?address=")}, 
    object::pair{std:("TOKEN_METADATA_ENDPOINT_BASE"), std:("/defi/v3/token/meta-data/single?address=")}, 
    object::pair{std:("MARKET_SEARCH_ENDPOINT_BASE"), std:("/defi/v3/token/trade-data/single?address=")}, 
    object::pair{std:("TOKEN_PRICE_CHANGE_ENDPOINT_BASE"), std:("/defi/v3/search?chain=base&target=token&sort_by=price_change_24h_percent&sort_type=desc&offset=0&limit=20")}, 
    object::pair{std:("TOKEN_VOLUME_24_ENDPOINT_BASE"), std:("/defi/v3/search?chain=base&target=token&sort_by=volume_24h_usd&sort_type=desc&offset=2&limit=20")}, 
    object::pair{std:("TOKEN_BUY_24_ENDPOINT_BASE"), std:("/defi/v3/search?chain=base&target=token&sort_by=buy_24h&sort_type=desc&offset=2&limit=20")}, 
    object::pair{std:("MAX_RETRIES"), 3}, 
    object::pair{std:("RETRY_DELAY"), 2000}
};
object CHAIN_CONFIG = object{
    object::pair{std:("SOLANA_ENABLED"), false}, 
    object::pair{std:("BASE_ENABLED"), true}
};
object BASE_CONFIG = object{
    object::pair{std:("RPC_URL"), OR((process->env->EVM_PROVIDER_URL), (std:("https://mainnet.base.org")))}, 
    object::pair{std:("ROUTER_ADDRESS"), std:("0x327Df1E6de05895d2ab08513aaDD9313Fe505d86")}, 
    object::pair{std:("WETH_ADDRESS"), std:("0x4200000000000000000000000000000000000006")}, 
    object::pair{std:("CHAIN_ID"), 8453}, 
    object::pair{std:("AERODROME"), object{
        object::pair{std:("WETH"), std:("0x4200000000000000000000000000000000000006")}, 
        object::pair{std:("USDC"), std:("0x833589fCD6eDb6E08f4c7C32D4f71b54bdA02913")}, 
        object::pair{std:("USDT"), std:("0x50c5725949A6F0c72E6C4a641F24049A917DB0Cb")}
    }}
};
object ZEROEX_CONFIG = object{
    object::pair{std:("API_URL"), std:("https://api.0x.org")}, 
    object::pair{std:("API_KEY"), OR((process->env->ZEROEX_API_KEY), (string_empty))}, 
    object::pair{std:("QUOTE_ENDPOINT"), std:("/swap/permit2/quote")}, 
    object::pair{std:("PRICE_ENDPOINT"), std:("/swap/permit2/price")}, 
    object::pair{std:("SUPPORTED_CHAINS"), object{
        object::pair{std:("BASE"), 8453}
    }}, 
    object::pair{std:("HEADERS"), object{
        object::pair{std:("Content-Type"), std:("application/json")}, 
        object::pair{std:("0x-api-key"), OR((process->env->ZEROEX_API_KEY), (string_empty))}, 
        object::pair{std:("0x-version"), std:("v2")}
    }}
};
object REQUIRED_SETTINGS = object{
    object::pair{std:("SOLANA_RPC_URL"), std:("Solana RPC endpoint URL")}, 
    object::pair{std:("SOLANA_PRIVATE_KEY"), std:("Private key for trading wallet")}, 
    object::pair{std:("BIRDEYE_API_KEY"), std:("API key for Birdeye data")}, 
    object::pair{std:("TRADER_SELL_KUMA"), std:("Webhook URL for sell notifications")}
};
object DEFAULT_CONFIG = object{
    object::pair{std:("intervals"), object{
        object::pair{std:("priceCheck"), 60000}, 
        object::pair{std:("walletSync"), 600000}, 
        object::pair{std:("performanceMonitor"), 3600000}
    }}, 
    object::pair{std:("thresholds"), object{
        object::pair{std:("minLiquidity"), 50000}, 
        object::pair{std:("minVolume"), 100000}, 
        object::pair{std:("minScore"), 60}
    }}, 
    object::pair{std:("riskLimits"), object{
        object::pair{std:("maxPositionSize"), 0.2}, 
        object::pair{std:("maxDrawdown"), 0.1}, 
        object::pair{std:("stopLossPercentage"), 0.05}, 
        object::pair{std:("takeProfitPercentage"), 0.2}
    }}, 
    object::pair{std:("slippageSettings"), object{
        object::pair{std:("baseSlippage"), 0.5}, 
        object::pair{std:("maxSlippage"), 1}, 
        object::pair{std:("liquidityMultiplier"), 1}, 
        object::pair{std:("volumeMultiplier"), 1}
    }}
};
object SAFETY_LIMITS = object{
    object::pair{std:("MINIMUM_TRADE"), 0.1}, 
    object::pair{std:("MAX_SLIPPAGE"), 0.05}, 
    object::pair{std:("MIN_LIQUIDITY"), 50000}, 
    object::pair{std:("MIN_VOLUME"), 10000}, 
    object::pair{std:("MAX_PRICE_CHANGE"), 30}
};

void Main(void)
{
}

MAIN
