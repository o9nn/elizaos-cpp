#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/config/providers.h"

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

void Main(void)
{
}

MAIN
